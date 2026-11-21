#ifndef GARAGE_UI_BUTTON_UI_HPP
#define GARAGE_UI_BUTTON_UI_HPP

#include "../actors/component.hpp"

#include "../core/common.hpp"

namespace Garage
{
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    class MaterialContent;
#endif
    class SystemContext;

    class GARAGE_API ButtonUI final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~ButtonUI() noexcept override = default;

        void OnUpdate(SystemContext& context) override;
        void OnDisable() override;
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
        void OnPostRender(SystemContext& context) override;

        [[nodiscard]] const MaterialContent* GetDebugMaterial() const noexcept;
        void                                 SetDebugMaterial(const MaterialContent* value) noexcept;
#endif

        [[nodiscard]] bool IsHovered() const noexcept;
        [[nodiscard]] bool IsPressed() const noexcept;

        [[nodiscard]] Delegate<>& OnEnter() noexcept;
        [[nodiscard]] Delegate<>& OnExit() noexcept;
        [[nodiscard]] Delegate<>& OnDown() noexcept;
        [[nodiscard]] Delegate<>& OnUp() noexcept;
        [[nodiscard]] Delegate<>& OnClick() noexcept;

    private:
        Delegate<> onEnter{};
        Delegate<> onExit{};
        Delegate<> onDown{};
        Delegate<> onUp{};
        Delegate<> onClick{};

        bool isHovered = false;
        bool isPressed = false;
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
        const MaterialContent* debugMaterial = nullptr;
#endif
    };

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    inline const MaterialContent* ButtonUI::GetDebugMaterial() const noexcept
    {
        return debugMaterial;
    }

    inline void ButtonUI::SetDebugMaterial(const MaterialContent* value) noexcept
    {
        debugMaterial = value;
    }
#endif

    inline bool ButtonUI::IsHovered() const noexcept
    {
        return isHovered;
    }

    inline bool ButtonUI::IsPressed() const noexcept
    {
        return isPressed;
    }

    inline Delegate<>& ButtonUI::OnEnter() noexcept
    {
        return onEnter;
    }

    inline Delegate<>& ButtonUI::OnExit() noexcept
    {
        return onExit;
    }

    inline Delegate<>& ButtonUI::OnDown() noexcept
    {
        return onDown;
    }

    inline Delegate<>& ButtonUI::OnUp() noexcept
    {
        return onUp;
    }

    inline Delegate<>& ButtonUI::OnClick() noexcept
    {
        return onClick;
    }
} // namespace Garage

#endif // GARAGE_UI_BUTTON_UI_HPP
