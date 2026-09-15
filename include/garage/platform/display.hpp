#ifndef GARAGE_PLATFORM_DISPLAY
#define GARAGE_PLATFORM_DISPLAY

#include <string>
#include <string_view>

namespace Garage
{
    class Display
    {
    public:
        Display() = default;
        virtual ~Display() = default;

        Display(const Display&) = delete;
        Display& operator=(const Display&) = delete;

        Display(Display&&) = default;
        Display& operator=(Display&&) = default;

        [[nodiscard]] virtual std::string GetCaption() const = 0;
        virtual void SetCaption(std::string_view caption) = 0;

        [[nodiscard]] virtual int GetSizeX() const = 0;
        virtual void SetSizeX(int sizeX) = 0;

        [[nodiscard]] virtual int GetSizeY() const = 0;
        virtual void SetSizeY(int sizeY) = 0;

        [[nodiscard]] virtual int GetPositionX() const = 0;
        virtual void SetPositionX(int positionX) = 0;

        [[nodiscard]] virtual int GetPositionY() const = 0;
        virtual void SetPositionY(int positionY) = 0;
    };
}

#endif
