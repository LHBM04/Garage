#include "garage/resources/shader_asset_importer.hpp"

#include <charconv>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <utility>

#include <rapidjson/document.h>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_shader.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/mesh_vertex.hpp"

#include "garage/resources/shader_asset.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    namespace
    {
        struct ShaderMetadata final
        {
            std::optional<EGPUShaderStage> stage = std::nullopt;
            String                         entryPoint{"main"};
            UInt32                         samplerCount        = 0u;
            UInt32                         storageTextureCount = 0u;
            UInt32                         storageBufferCount  = 0u;
            UInt32                         uniformBufferCount  = 0u;
            EGPUCullMode                   cullMode            = EGPUCullMode::BACK;
            EGPUFillMode                   fillMode            = EGPUFillMode::FILL;
            EGPUFrontFace                  frontFace           = EGPUFrontFace::CLOCKWISE;
            bool                           enableDepthTest     = true;
            bool                           enableDepthWrite    = true;
            EGPUCompareOp                  depthCompareOp      = EGPUCompareOp::LESS_OR_EQUAL;
            bool                           alphaBlend          = false;
            EGPUPrimitiveTopology          primitive           = EGPUPrimitiveTopology::TRIANGLE_LIST;
            Vector<GPUVertexAttribute>     vertexAttributes;
            UInt32                         vertexStride        = sizeof(MeshVertex);
            bool                           hasCustomVertexDecl = false;
        };

        [[nodiscard]] constexpr bool IsAsciiWhitespace(char ch) noexcept
        {
            return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
        }

        [[nodiscard]] StringView Trim(StringView text) noexcept
        {
            constexpr StringView UTF8_BOM = "\xEF\xBB\xBF";
            if (text.starts_with(UTF8_BOM))
            {
                text.remove_prefix(UTF8_BOM.size());
            }
            while (!text.empty() && IsAsciiWhitespace(text.front()))
            {
                text.remove_prefix(1);
            }
            while (!text.empty() && IsAsciiWhitespace(text.back()))
            {
                text.remove_suffix(1);
            }
            return text;
        }

        [[nodiscard]] bool EqualsIgnoreCase(StringView lhs, StringView rhs) noexcept
        {
            if (lhs.size() != rhs.size())
            {
                return false;
            }

            for (USize i = 0; i < lhs.size(); ++i)
            {
                if (std::tolower(static_cast<unsigned char>(lhs[i])) !=
                    std::tolower(static_cast<unsigned char>(rhs[i])))
                {
                    return false;
                }
            }

            return true;
        }

        template <class TValue, USize N>
        [[nodiscard]] bool
                TryMapValue(StringView name, const Array<std::pair<StringView, TValue>, N>& table, TValue& out) noexcept
        {
            for (const auto& [key, value] : table)
            {
                if (EqualsIgnoreCase(key, name))
                {
                    out = value;
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] bool TryParseUInt32(StringView text, UInt32& out) noexcept
        {
            UInt32 parsed        = 0u;
            const auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), parsed);
            if (ec == std::errc{} && ptr == text.data() + text.size())
            {
                out = parsed;
                return true;
            }

            return false;
        }

        [[nodiscard]] EGPUShaderFormat DetectShaderFormat(const std::filesystem::path& path)
        {
            String extension(path.extension().string());
            for (char& ch : extension)
            {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }

            if (extension == ".spv")
            {
                return EGPUShaderFormat::SPIRV;
            }
            if (extension == ".dxil")
            {
                return EGPUShaderFormat::DXIL;
            }
            if (extension == ".msl")
            {
                return EGPUShaderFormat::MSL;
            }
            if (extension == ".dxbc")
            {
                return EGPUShaderFormat::DXBC;
            }
            if (extension == ".metallib")
            {
                return EGPUShaderFormat::METALLIB;
            }

            return EGPUShaderFormat::INVALID;
        }

        [[nodiscard]] std::optional<EGPUShaderStage> InferStageFromFilename(const std::filesystem::path& path)
        {
            const String     STEM(path.stem().string());
            const StringView VIEW(STEM);

            static constexpr Array<StringView, 4> VERTEX_SUFFIXES{".vs", "_vs", ".vert", "_vert"};
            static constexpr Array<StringView, 6> FRAGMENT_SUFFIXES{".ps", "_ps", ".fs", "_fs", ".frag", "_frag"};

            for (const StringView SUFFIX : VERTEX_SUFFIXES)
            {
                if (VIEW.size() >= SUFFIX.size() && EqualsIgnoreCase(VIEW.substr(VIEW.size() - SUFFIX.size()), SUFFIX))
                {
                    return EGPUShaderStage::VERTEX;
                }
            }

            for (const StringView SUFFIX : FRAGMENT_SUFFIXES)
            {
                if (VIEW.size() >= SUFFIX.size() && EqualsIgnoreCase(VIEW.substr(VIEW.size() - SUFFIX.size()), SUFFIX))
                {
                    return EGPUShaderStage::FRAGMENT;
                }
            }

            return std::nullopt;
        }

        void ParseVertexInputSpec(StringView value, ShaderMetadata& metadata)
        {
            metadata.hasCustomVertexDecl = true;
            metadata.vertexAttributes.clear();
            metadata.vertexStride = 0u;

            if (EqualsIgnoreCase(value, "None"))
            {
                return;
            }

            if (EqualsIgnoreCase(value, "Mesh") || EqualsIgnoreCase(value, "All"))
            {
                metadata.vertexAttributes.assign(MESH_VERTEX_ATTRIBUTES.begin(), MESH_VERTEX_ATTRIBUTES.end());
                metadata.vertexStride = sizeof(MeshVertex);
                return;
            }

            USize cursor = 0u;
            while (cursor <= value.size())
            {
                const USize      COMMA = value.find(',', cursor);
                const StringView TOKEN =
                        Trim((COMMA == StringView::npos) ? value.substr(cursor) : value.substr(cursor, COMMA - cursor));

                if (EqualsIgnoreCase(TOKEN, "Position"))
                {
                    metadata.vertexAttributes.push_back(MESH_VERTEX_ATTRIBUTES[0]);
                }
                else if (EqualsIgnoreCase(TOKEN, "Normal"))
                {
                    metadata.vertexAttributes.push_back(MESH_VERTEX_ATTRIBUTES[1]);
                }
                else if (EqualsIgnoreCase(TOKEN, "Color"))
                {
                    metadata.vertexAttributes.push_back(MESH_VERTEX_ATTRIBUTES[2]);
                }
                else if (EqualsIgnoreCase(TOKEN, "TexCoord") || EqualsIgnoreCase(TOKEN, "UV"))
                {
                    metadata.vertexAttributes.push_back(MESH_VERTEX_ATTRIBUTES[3]);
                }

                if (COMMA == StringView::npos)
                {
                    break;
                }
                cursor = COMMA + 1u;
            }

            if (!metadata.vertexAttributes.empty())
            {
                metadata.vertexStride = sizeof(MeshVertex);
            }
        }

        void ApplyMetadataEntry(StringView key, StringView value, ShaderMetadata& metadata)
        {
            static constexpr Array<std::pair<StringView, EGPUCullMode>, 3>          CULL_MODES{{
                    {"Back", EGPUCullMode::BACK},
                    {"Front", EGPUCullMode::FRONT},
                    {"None", EGPUCullMode::NONE},
            }};
            static constexpr Array<std::pair<StringView, EGPUFillMode>, 4>          FILL_ALIASES{{
                    {"Solid", EGPUFillMode::FILL},
                    {"Fill", EGPUFillMode::FILL},
                    {"Wireframe", EGPUFillMode::LINE},
                    {"Line", EGPUFillMode::LINE},
            }};
            static constexpr Array<std::pair<StringView, EGPUFrontFace>, 2>         FRONT_FACES{{
                    {"Clockwise", EGPUFrontFace::CLOCKWISE},
                    {"CounterClockwise", EGPUFrontFace::COUNTER_CLOCKWISE},
            }};
            static constexpr Array<std::pair<StringView, EGPUCompareOp>, 8>         DEPTH_FUNCS{{
                    {"Never", EGPUCompareOp::NEVER},
                    {"Less", EGPUCompareOp::LESS},
                    {"Equal", EGPUCompareOp::EQUAL},
                    {"LessEqual", EGPUCompareOp::LESS_OR_EQUAL},
                    {"Greater", EGPUCompareOp::GREATER},
                    {"NotEqual", EGPUCompareOp::NOT_EQUAL},
                    {"GreaterEqual", EGPUCompareOp::GREATER_OR_EQUAL},
                    {"Always", EGPUCompareOp::ALWAYS},
            }};
            static constexpr Array<std::pair<StringView, EGPUPrimitiveTopology>, 5> TOPOLOGIES{{
                    {"TriangleList", EGPUPrimitiveTopology::TRIANGLE_LIST},
                    {"TriangleStrip", EGPUPrimitiveTopology::TRIANGLE_STRIP},
                    {"LineList", EGPUPrimitiveTopology::LINE_LIST},
                    {"LineStrip", EGPUPrimitiveTopology::LINE_STRIP},
                    {"PointList", EGPUPrimitiveTopology::POINT_LIST},
            }};
            static constexpr Array<std::pair<StringView, EGPUShaderStage>, 5>       STAGES{{
                    {"Vertex", EGPUShaderStage::VERTEX},
                    {"VS", EGPUShaderStage::VERTEX},
                    {"Fragment", EGPUShaderStage::FRAGMENT},
                    {"Pixel", EGPUShaderStage::FRAGMENT},
                    {"PS", EGPUShaderStage::FRAGMENT},
            }};

            if (EqualsIgnoreCase(key, "stage"))
            {
                EGPUShaderStage parsedStage = EGPUShaderStage::VERTEX;
                if (TryMapValue(value, STAGES, parsedStage))
                {
                    metadata.stage = parsedStage;
                }
            }
            else if (EqualsIgnoreCase(key, "entryPoint"))
            {
                if (!value.empty())
                {
                    metadata.entryPoint.assign(value);
                }
            }
            else if (EqualsIgnoreCase(key, "samplers") || EqualsIgnoreCase(key, "samplerCount"))
            {
                (void)TryParseUInt32(value, metadata.samplerCount);
            }
            else if (EqualsIgnoreCase(key, "storageTextures") || EqualsIgnoreCase(key, "storageTextureCount"))
            {
                (void)TryParseUInt32(value, metadata.storageTextureCount);
            }
            else if (EqualsIgnoreCase(key, "storageBuffers") || EqualsIgnoreCase(key, "storageBufferCount"))
            {
                (void)TryParseUInt32(value, metadata.storageBufferCount);
            }
            else if (EqualsIgnoreCase(key, "uniformBuffers") || EqualsIgnoreCase(key, "uniformBufferCount"))
            {
                (void)TryParseUInt32(value, metadata.uniformBufferCount);
            }
            else if (EqualsIgnoreCase(key, "cull"))
            {
                (void)TryMapValue(value, CULL_MODES, metadata.cullMode);
            }
            else if (EqualsIgnoreCase(key, "fill"))
            {
                (void)TryMapValue(value, FILL_ALIASES, metadata.fillMode);
            }
            else if (EqualsIgnoreCase(key, "frontFace"))
            {
                (void)TryMapValue(value, FRONT_FACES, metadata.frontFace);
            }
            else if (EqualsIgnoreCase(key, "depth"))
            {
                if (EqualsIgnoreCase(value, "Off"))
                {
                    metadata.enableDepthTest = false;
                }
                else if (EqualsIgnoreCase(value, "On"))
                {
                    metadata.enableDepthTest = true;
                    metadata.depthCompareOp  = EGPUCompareOp::LESS_OR_EQUAL;
                }
                else if (TryMapValue(value, DEPTH_FUNCS, metadata.depthCompareOp))
                {
                    metadata.enableDepthTest = true;
                }
            }
            else if (EqualsIgnoreCase(key, "depthWrite"))
            {
                if (EqualsIgnoreCase(value, "Off"))
                {
                    metadata.enableDepthWrite = false;
                }
                else if (EqualsIgnoreCase(value, "On"))
                {
                    metadata.enableDepthWrite = true;
                }
            }
            else if (EqualsIgnoreCase(key, "blend"))
            {
                if (EqualsIgnoreCase(value, "Alpha"))
                {
                    metadata.alphaBlend = true;
                }
                else if (EqualsIgnoreCase(value, "Opaque"))
                {
                    metadata.alphaBlend = false;
                }
            }
            else if (EqualsIgnoreCase(key, "topology"))
            {
                (void)TryMapValue(value, TOPOLOGIES, metadata.primitive);
            }
            else if (EqualsIgnoreCase(key, "vertexInput") || EqualsIgnoreCase(key, "vertexInputs"))
            {
                ParseVertexInputSpec(value, metadata);
            }
        }

        void LoadJsonSidecar(const std::filesystem::path& jsonPath, ShaderMetadata& metadata)
        {
            if (!std::filesystem::exists(jsonPath))
            {
                return;
            }

            auto documentResult = JsonUtility::LoadDocument(jsonPath);
            if (!documentResult)
            {
                return;
            }

            const rapidjson::Document& DOCUMENT = *documentResult;
            for (auto it = DOCUMENT.MemberBegin(); it != DOCUMENT.MemberEnd(); ++it)
            {
                const StringView KEY(it->name.GetString(), it->name.GetStringLength());
                if (it->value.IsString())
                {
                    const StringView VALUE(it->value.GetString(), it->value.GetStringLength());
                    ApplyMetadataEntry(KEY, VALUE, metadata);
                }
                else if (it->value.IsUint())
                {
                    const UInt32 NUMBER = it->value.GetUint();
                    if (EqualsIgnoreCase(KEY, "samplers") || EqualsIgnoreCase(KEY, "samplerCount"))
                    {
                        metadata.samplerCount = NUMBER;
                    }
                    else if (EqualsIgnoreCase(KEY, "storageTextures") || EqualsIgnoreCase(KEY, "storageTextureCount"))
                    {
                        metadata.storageTextureCount = NUMBER;
                    }
                    else if (EqualsIgnoreCase(KEY, "storageBuffers") || EqualsIgnoreCase(KEY, "storageBufferCount"))
                    {
                        metadata.storageBufferCount = NUMBER;
                    }
                    else if (EqualsIgnoreCase(KEY, "uniformBuffers") || EqualsIgnoreCase(KEY, "uniformBufferCount"))
                    {
                        metadata.uniformBufferCount = NUMBER;
                    }
                }
                else if (it->value.IsBool())
                {
                    const bool FLAG = it->value.GetBool();
                    if (EqualsIgnoreCase(KEY, "depthWrite"))
                    {
                        metadata.enableDepthWrite = FLAG;
                    }
                }
            }
        }

        void LoadShaderMetadata(const std::filesystem::path& shaderPath, ShaderMetadata& metadata)
        {
            std::filesystem::path baseStem = shaderPath.stem();
            if (baseStem.has_extension())
            {
                baseStem = baseStem.stem();
            }

            const std::filesystem::path SHARED_JSON = shaderPath.parent_path() / (baseStem.string() + ".json");
            const std::filesystem::path EXACT_JSON  = std::filesystem::path(shaderPath.string() + ".json");

            LoadJsonSidecar(SHARED_JSON, metadata);
            LoadJsonSidecar(EXACT_JSON, metadata);
        }
    } // namespace

    Result<UniquePtr<ShaderAsset>, String> ShaderAssetImporter::Import(const std::filesystem::path& path) const
    {
        const EGPUShaderFormat FORMAT = DetectShaderFormat(path);
        if (FORMAT == EGPUShaderFormat::INVALID)
        {
            return Result<UniquePtr<ShaderAsset>, String>::Err(
                    String("셰이더 로더에는 .spv, .dxil, .msl, .dxbc 또는 .metallib 파일이 필요합니다."));
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            String error("셰이더 파일을 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<ShaderAsset>, String>::Err(std::move(error));
        }

        const std::streamsize FILE_SIZE = file.tellg();
        if (FILE_SIZE <= 0)
        {
            String error("셰이더 파일이 비어 있습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<ShaderAsset>, String>::Err(std::move(error));
        }

        file.seekg(0, std::ios::beg);
        Vector<Byte> bytecode(static_cast<USize>(FILE_SIZE));
        if (!file.read(reinterpret_cast<char*>(bytecode.data()), FILE_SIZE))
        {
            String error("셰이더 바이트코드 읽기에 실패했습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<ShaderAsset>, String>::Err(std::move(error));
        }

        ShaderMetadata metadata{};
        LoadShaderMetadata(path, metadata);

        if (!metadata.stage.has_value())
        {
            metadata.stage = InferStageFromFilename(path);
        }

        if (!metadata.stage.has_value())
        {
            String error("셰이더 스테이지(Vertex/Fragment)를 판별할 수 없습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<ShaderAsset>, String>::Err(std::move(error));
        }

        if (*metadata.stage == EGPUShaderStage::VERTEX && !metadata.hasCustomVertexDecl)
        {
            metadata.vertexAttributes.assign(MESH_VERTEX_ATTRIBUTES.begin(), MESH_VERTEX_ATTRIBUTES.end());
            metadata.vertexStride = sizeof(MeshVertex);
        }

        const GPUShaderCreateInfo CREATE_INFO{
                .code                = bytecode,
                .entryPoint          = metadata.entryPoint,
                .format              = FORMAT,
                .stage               = *metadata.stage,
                .samplerCount        = metadata.samplerCount,
                .storageTextureCount = metadata.storageTextureCount,
                .storageBufferCount  = metadata.storageBufferCount,
                .uniformBufferCount  = metadata.uniformBufferCount,
        };

        auto shaderResult = device.CreateShader(CREATE_INFO);
        if (!shaderResult)
        {
            return Result<UniquePtr<ShaderAsset>, String>::Err(std::move(shaderResult).GetError());
        }

        UniquePtr<ShaderAsset> asset = MakeUnique<ShaderAsset>(path, device, shaderResult.GetValue());
        asset->SetVertexAttributes(std::move(metadata.vertexAttributes));
        asset->SetVertexStride(metadata.vertexStride);
        asset->SetCullMode(metadata.cullMode);
        asset->SetFillMode(metadata.fillMode);
        asset->SetFrontFace(metadata.frontFace);
        asset->SetDepthTestEnabled(metadata.enableDepthTest);
        asset->SetDepthWriteEnabled(metadata.enableDepthWrite);
        asset->SetDepthCompareOp(metadata.depthCompareOp);
        if (metadata.alphaBlend)
        {
            asset->SetAlphaBlend();
        }
        else
        {
            asset->ResetOpaqueBlend();
        }
        asset->SetPrimitive(metadata.primitive);

        return Result<UniquePtr<ShaderAsset>, String>::Ok(std::move(asset));
    }
} // namespace Garage
