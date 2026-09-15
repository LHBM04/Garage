#ifndef GARAGE_PLATFORM_DISPLAYINTERNAL
#define GARAGE_PLATFORM_DISPLAYINTERNAL

#include <string>
#include <string_view>

#include <SDL3/SDL_video.h>

#include "garage/platform/display.hpp"

namespace Garage
{
    class DisplayInternal : public Display
    {
    public:
        explicit DisplayInternal(SDL_Window* window);
        ~DisplayInternal() override;

        DisplayInternal(const DisplayInternal&) = delete;
        DisplayInternal& operator=(const DisplayInternal&) = delete;

        DisplayInternal(DisplayInternal&&) noexcept ;
        DisplayInternal& operator=(DisplayInternal&&) noexcept ;

        [[nodiscard]] std::string GetCaption() const override;
        void SetCaption(std::string_view caption) override;

        [[nodiscard]] int GetSizeX() const override;
        void SetSizeX(int sizeX) override;

        [[nodiscard]] int GetSizeY() const override;
        void SetSizeY(int sizeY) override;

        [[nodiscard]] int GetPositionX() const override;
        void SetPositionX(int positionX) override;

        [[nodiscard]] int GetPositionY() const override;
        void SetPositionY(int positionY) override;

    private:
        SDL_Window* window;
    };
}

#endif
