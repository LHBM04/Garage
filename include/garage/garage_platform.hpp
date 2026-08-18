#pragma once

#include <string>

namespace Garage
{
	/**
	 * @brief 창 옵션.
	 */
	struct WindowOptions final
	{
		/**
		 * @brief 창의 제목.
		 */
		std::string title;

		/**
		 * @brief 창의 X 좌표.
		 */
		int positionX;

		/**
		 * @brief 창의 Y 좌표.
		 */
		int positionY;

		/**
		 * @brief 창의 너비.
		 */
		int sizeX;

		/**
		 * @brief 창의 높이.
		 */
		int sizeY;
	};

	/**
	 * @brief 창.
	 */
	class Window
	{
	public:
		virtual ~Window() noexcept = default;

		/**
		 * @brief 창을 생성합니다.
		 * @param options 생성할 창의 옵션
		 * @return 옵션에 따라 생성된 창
		 */
		[[nodiscard]] static Window* Create(const WindowOptions& options);

		[[nodiscard]] virtual std::string GetTitle() const noexcept = 0;
		virtual Window& SetTitle(const std::string& title) noexcept = 0;

		[[nodiscard]] virtual int GetPositionX() const noexcept = 0;
		virtual Window& SetPositionX(int x_) noexcept = 0;

		[[nodiscard]] virtual int GetPositionY() const noexcept = 0;
		virtual Window& SetPositionY(int y_) noexcept = 0;

		[[nodiscard]] virtual int GetSizeX() const noexcept = 0;
		virtual Window& SetSizeX(int x_) noexcept = 0;

		[[nodiscard]] virtual int GetSizeY() const noexcept = 0;
		virtual Window& SetSizeY(int y_) noexcept = 0;
	};
}
