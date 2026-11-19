#include "garage/ui/text_view.hpp"

#include <array>
#include <cmath>
#include <span>
#include <string_view>

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"

#include "garage/core/common.hpp"
#include "garage/core/system_context.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/render_subsystem.hpp"

#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector4d.hpp"

#include "garage/resources/bitmap_font_asset.hpp"
#include "garage/resources/material_asset.hpp"
#include "garage/resources/texture_asset.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] UInt32 DecodeUTF8(std::string_view text, USize& index) noexcept
        {
            const UInt8 LEAD = static_cast<UInt8>(text[index]);
            if (LEAD < 0x80u)
            {
                ++index;
                return LEAD;
            }

            if ((LEAD & 0xE0u) == 0xC0u && index + 1 < text.size())
            {
                const UInt32 CODE_POINT = ((static_cast<UInt32>(LEAD) & 0x1Fu) << 6u) |
                        (static_cast<UInt32>(static_cast<UInt8>(text[index + 1])) & 0x3Fu);
                index += 2;
                return CODE_POINT;
            }

            if ((LEAD & 0xF0u) == 0xE0u && index + 2 < text.size())
            {
                const UInt32 CODE_POINT = ((static_cast<UInt32>(LEAD) & 0x0Fu) << 12u) |
                        ((static_cast<UInt32>(static_cast<UInt8>(text[index + 1])) & 0x3Fu) << 6u) |
                        (static_cast<UInt32>(static_cast<UInt8>(text[index + 2])) & 0x3Fu);
                index += 3;
                return CODE_POINT;
            }

            if ((LEAD & 0xF8u) == 0xF0u && index + 3 < text.size())
            {
                const UInt32 CODE_POINT = ((static_cast<UInt32>(LEAD) & 0x07u) << 18u) |
                        ((static_cast<UInt32>(static_cast<UInt8>(text[index + 1])) & 0x3Fu) << 12u) |
                        ((static_cast<UInt32>(static_cast<UInt8>(text[index + 2])) & 0x3Fu) << 6u) |
                        (static_cast<UInt32>(static_cast<UInt8>(text[index + 3])) & 0x3Fu);
                index += 4;
                return CODE_POINT;
            }

            ++index;
            return LEAD;
        }
    } // namespace

    void TextView::SetFont(const BitmapFontAsset* value) noexcept
    {
        font = value;
        characterMap.clear();

        if (font == nullptr)
        {
            return;
        }

        const std::span<const FontCharacterData> CHARACTERS = font->GetCharacters();
        characterMap.reserve(CHARACTERS.size());

        for (const FontCharacterData& character : CHARACTERS)
        {
            characterMap.emplace(character.unicode, &character);
        }
    }

    const FontCharacterData* TextView::GetCharacter(UInt32 unicode) const noexcept
    {
        const auto FOUND = characterMap.find(unicode);
        if (FOUND == characterMap.end())
        {
            return nullptr;
        }

        return FOUND->second;
    }

    void TextView::OnPostRender(SystemContext& context)
    {
        RenderSubsystem*   renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
        IGPUCommandBuffer* commandBuffer =
                (renderSubsystem != nullptr) ? renderSubsystem->GetGPUCommandBuffer() : nullptr;
        if (commandBuffer == nullptr || texture == nullptr || font == nullptr || material == nullptr || text.empty())
        {
            return;
        }

        const IGPUTexture2D*             gpuTexture    = texture->GetTexture();
        const IGPUGraphicsPipelineState* pipelineState = material->GetGraphicsPipelineState();
        if (gpuTexture == nullptr || pipelineState == nullptr)
        {
            return;
        }

        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        if (transform == nullptr)
        {
            LOG_ERROR("UI 텍스트 액터에 Transform 컴포넌트가 필요합니다.");
            return;
        }

        ObjectConstants objectConstants{
                .world = transform->GetWorldMatrix(),
        };

        const Float32     SCALE_X   = std::hypot(objectConstants.world[0], objectConstants.world[1]);
        const Float32     SCALE_Y   = std::hypot(objectConstants.world[4], objectConstants.world[5]);
        constexpr Float32 MIN_SCALE = 1.0e-6f;
        if (SCALE_X > MIN_SCALE)
        {
            objectConstants.world[0] /= SCALE_X;
            objectConstants.world[1] /= SCALE_X;
        }
        if (SCALE_Y > MIN_SCALE)
        {
            objectConstants.world[4] /= SCALE_Y;
            objectConstants.world[5] /= SCALE_Y;
        }

        if (const auto RESULT = commandBuffer->SetGraphicsConstants(objectConstants); !RESULT)
        {
            LOG_ERROR("UI 텍스트 변환을 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        std::array<const IGPUTexture2D*, IGPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
        textures[0] = gpuTexture;
        if (const auto RESULT = commandBuffer->BindTextures(textures); !RESULT)
        {
            LOG_ERROR("UI 텍스트 텍스처를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        commandBuffer->SetGPUGraphicsPipelineState(*pipelineState);
        commandBuffer->SetPrimitiveTopology(material->GetPrimitive());

        const GPUViewport&  VIEWPORT      = commandBuffer->GetActiveViewport();
        const Vector2DFloat VIEWPORT_SIZE = (VIEWPORT.width > 0.0f && VIEWPORT.height > 0.0f)
                ? Vector2DFloat{VIEWPORT.width, VIEWPORT.height}
                : Vector2DFloat{1280.0f, 960.0f};

        const Float32 POINT_SIZE  = font->GetPointSize();
        const Float32 SCALE       = (POINT_SIZE > 0.0f) ? (fontSize / POINT_SIZE) : 1.0f;
        const Float32 LINE_HEIGHT = font->GetLineHeight() * SCALE;

        const auto GET_LINE_START_X = [this, SCALE, SCALE_X](USize startIndex) -> Float32
        {
            if (horizontalAlignment == ETextHorizontalAlignment::LEFT)
            {
                return -SCALE_X * 0.5f;
            }

            Float32 lineWidth = 0.0f;
            while (startIndex < text.size())
            {
                const UInt32 CODE_POINT = DecodeUTF8(text, startIndex);
                if (CODE_POINT == U'\n')
                {
                    break;
                }
                if (CODE_POINT == U'\r')
                {
                    continue;
                }
                if (const FontCharacterData* character = GetCharacter(CODE_POINT); character != nullptr)
                {
                    lineWidth += character->advance * SCALE;
                }
            }

            return (horizontalAlignment == ETextHorizontalAlignment::CENTER) ? (-lineWidth * 0.5f)
                                                                             : (SCALE_X * 0.5f - lineWidth);
        };

        Float32 cursorX   = GET_LINE_START_X(0);
        Float32 baselineY = -SCALE_Y * 0.5f + font->GetAscentLine() * SCALE;

        constexpr UInt32 TEXT_VERTEX_COUNT = 4;

        USize index = 0;
        while (index < text.size())
        {
            const UInt32 CODE_POINT = DecodeUTF8(text, index);
            if (CODE_POINT == U'\r')
            {
                continue;
            }
            if (CODE_POINT == U'\n')
            {
                cursorX = GET_LINE_START_X(index);
                baselineY += LINE_HEIGHT;
                continue;
            }

            const FontCharacterData* character = GetCharacter(CODE_POINT);
            if (character == nullptr)
            {
                continue;
            }

            const Float32 GLYPH_WIDTH  = character->width * SCALE;
            const Float32 GLYPH_HEIGHT = character->height * SCALE;
            if (GLYPH_WIDTH > 0.0f && GLYPH_HEIGHT > 0.0f)
            {
                const Float32 GLYPH_X = cursorX + character->bearingX * SCALE;
                const Float32 GLYPH_Y = baselineY - character->bearingY * SCALE;

                const TextUIConstants CONSTANTS{
                        .viewportSize = VIEWPORT_SIZE,
                        .padding      = Vector2DFloat{},
                        .glyphRect    = Vector4DFloat{GLYPH_X, GLYPH_Y, GLYPH_WIDTH, GLYPH_HEIGHT},
                        .glyphUV      = Vector4DFloat{character->uvMin[0],
                                                      character->uvMin[1],
                                                      character->uvMax[0],
                                                      character->uvMax[1]},
                        .textColor    = color,
                };

                if (const auto RESULT = commandBuffer->SetGraphicsConstants(CONSTANTS); !RESULT)
                {
                    LOG_ERROR("UI 텍스트 상수를 바인딩하지 못했습니다: {}", RESULT.GetError());
                    return;
                }

                commandBuffer->Draw(TEXT_VERTEX_COUNT);
            }

            cursorX += character->advance * SCALE;
        }
    }
} // namespace Garage
