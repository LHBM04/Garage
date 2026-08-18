#pragma once

#include <string>
#include <string_view>

namespace Garage
{
    struct WindowOptions;
    
    /**
     * @class Window
     * @brief 애플리케이션 내 창을 정의합니다.
     */
    class Window
	{
	public:
		virtual ~Window() noexcept = default;

        /**
         * @brief 지정한 설정을 바탕으로 새 창을 생성합니다.
         * @param options 생성할 창의 설정
         * @return Window* 새 창
         */
        [[nodiscard]] static Window* Create(const WindowOptions& options);

        /**
         * @brief 해당 창의 제목을 가져옵니다.
         * @return std::string 해당 창의 제목
         */
        [[nodiscard]] virtual std::string GetTitle() const noexcept = 0;

        /**
         * @brief 해당 창의 제목을 지정한 제목으로 설정합니다.
         * @param title 지정할 제목
         * @return Window& 설정이 적용된 창
         */
        virtual Window& SetTitle(std::string_view title) noexcept = 0;

        /**
         * @brief 해당 창의 X 위치를 반환합니다.
         * @return int 해당 창의 X 위치
         */
        [[nodiscard]] virtual int GetPositionX() const noexcept = 0;

        /**
         * @brief 해당 창의 X 위치를 지정한 값으로 설정합니다.
         * @param x_ 지정할 값
         * @return Window& 설정이 적용된 창
         */
        virtual Window& SetPositionX(int x_) noexcept = 0;

        /**
         * @brief 해당 창의 Y 위치를 지정한 값으로 설정합니다.
         * @return int 해당 창의 Y 위치
         */
        [[nodiscard]] virtual int GetPositionY() const noexcept = 0;

        /**
         * @brief 해당 창의 Y 위치를 지정한 값으로 설정합니다.
         * @param y_ 지정할 값
         * @return Window& 설정이 적용된 창
         */
        virtual Window& SetPositionY(int y_) noexcept = 0;

        /**
         * @brief 해당 창의 가로 크기를 반환합니다.
         * @return int 해당 창의 가로 크기
         */
        [[nodiscard]] virtual int GetSizeX() const noexcept = 0;

        /**
         * @brief 해당 창의 세로 크기를 지정한 값으로 설정합니다.
         * @param x_ 지정할 값
         * @return Window& 설정이 적용된 창
         */
        virtual Window& SetSizeX(int x_) noexcept = 0;

        /**
         * @brief 해당 창의 세로 크기를 반환합니다.
         * @return int 해당 창의 세로 크기
         */
        [[nodiscard]] virtual int GetSizeY() const noexcept = 0;

        /**
         * @brief 해당 창의 새로 크기를 지정한 값으로 설정합니다.
         * @param y_ 지정할 값
         * @return Window& 값이 적용된 창
         */
        virtual Window& SetSizeY(int y_) noexcept = 0;
	};
}
