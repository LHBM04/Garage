#ifndef GARAGE_CONTENT_CONTENT_SUBSYSTEM_HPP
#define GARAGE_CONTENT_CONTENT_SUBSYSTEM_HPP

#include <concepts>
#include <filesystem>
#include <typeindex>
#include <utility>

#include "../core/app_subsystem.hpp"
#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"
#include "../core/system_status.hpp"

#include "../utilities/logger.hpp"

#include "content.hpp"
#include "content_importer.hpp"

namespace Garage
{
    class SystemContext;
    class WorldBase;

    class GARAGE_API ContentSubsystem final : public AppSubsystemBase
    {
    public:
        ContentSubsystem() noexcept;
        ~ContentSubsystem() noexcept override = default;

        template <std::derived_from<ContentImporter<>> TContentImporter, class... TArgs>
        void AddImporter(TArgs&&... args);

        template <std::derived_from<ContentBase> TContent>
        [[nodiscard]] const TContent* LoadContent(const std::filesystem::path& path);

        void UnloadContent(const std::filesystem::path& path);
        void UnloadAllContent() noexcept;

        void OnWorldEntered(const WorldBase* world);
        void OnWorldExited(const WorldBase* world);

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnEarlyUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnFixedUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnLateUpdate(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

    private:
        struct PathHash final
        {
            [[nodiscard]] USize operator()(const std::filesystem::path& path) const noexcept;
        };

        template <std::derived_from<ContentBase> TContent>
        [[nodiscard]] const TContent* GetLoadedContent(const std::filesystem::path& path) const noexcept;

        using ContentImporterList = Vector<UniquePtr<ContentImporter<>>>;
        using ContentImporterDict = UnorderedMap<std::type_index, ContentImporter<>*>;

        using ContentList = Vector<UniquePtr<ContentBase>>;
        using ContentDict = UnorderedMap<std::filesystem::path, const ContentBase*, PathHash>;

        ContentImporterList contentImporters;
        ContentImporterDict contentImportersByType;

        ContentList loadedContent;
        ContentDict contentByPath;

        Int32 worldExitListenerId = 0;
    };

    inline ContentSubsystem::ContentSubsystem() noexcept
        : AppSubsystemBase(10)
    {
    }

    inline USize ContentSubsystem::PathHash::operator()(const std::filesystem::path& path) const noexcept
    {
        return std::filesystem::hash_value(path);
    }

    template <std::derived_from<ContentImporter<>> TContentImporter, class... TArgs>
    void ContentSubsystem::AddImporter(TArgs&&... args)
    {
        const auto GET_CONTENT_TYPE =
                []<class TImporter, std::derived_from<ContentBase> TContent>(
                        Result<UniquePtr<TContent>, String> (TImporter::*)(const std::filesystem::path&) const) noexcept
        {
            return std::type_index(typeid(TContent));
        };

        const std::type_index CONTENT_TYPE = GET_CONTENT_TYPE(&TContentImporter::Import);
        if (contentImportersByType.contains(CONTENT_TYPE))
        {
            return;
        }

        UniquePtr<TContentImporter> importer = MakeUnique<TContentImporter>(this, std::forward<TArgs>(args)...);
        ContentImporter<>*          pointer  = importer.get();
        contentImporters.emplace_back(std::move(importer));
        contentImportersByType.emplace(CONTENT_TYPE, pointer);
    }

    template <std::derived_from<ContentBase> TContent>
    const TContent* ContentSubsystem::GetLoadedContent(const std::filesystem::path& path) const noexcept
    {
        const auto CACHED = contentByPath.find(path.lexically_normal());
        if (CACHED == contentByPath.end())
        {
            return nullptr;
        }

        return dynamic_cast<const TContent*>(CACHED->second);
    }

    template <std::derived_from<ContentBase> TContent>
    const TContent* ContentSubsystem::LoadContent(const std::filesystem::path& path)
    {
        const std::filesystem::path NORMALIZED_PATH = path.lexically_normal();
        if (const auto CACHED = contentByPath.find(NORMALIZED_PATH); CACHED != contentByPath.end())
        {
            return dynamic_cast<const TContent*>(CACHED->second);
        }

        const auto CONTENT_IMPORTER = contentImportersByType.find(std::type_index(typeid(TContent)));
        if (CONTENT_IMPORTER == contentImportersByType.end())
        {
            return nullptr;
        }

        const auto* importer = static_cast<const ContentImporter<TContent>*>(CONTENT_IMPORTER->second);
        auto        content  = importer->Import(NORMALIZED_PATH);
        if (!content)
        {
            LOG_ERROR("콘텐츠 로드에 실패했습니다: {}: {}", NORMALIZED_PATH.generic_string(), content.GetError());
            return nullptr;
        }

        const TContent* result = content.GetValue().get();
        if (result == nullptr)
        {
            return nullptr;
        }

        loadedContent.emplace_back(std::move(*content));
        contentByPath.emplace(NORMALIZED_PATH, result);
        LOG_INFO("콘텐츠를 로드했습니다: {}", NORMALIZED_PATH.generic_string());
        return result;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_CONTENT_SUBSYSTEM_HPP
