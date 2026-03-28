/**
 * @file renderer.hpp
 */
#ifndef __GARAGE_RENDERER_HPP__
#define __GARAGE_RENDERER_HPP__

namespace Garage
{
	class RenderDevice;

	namespace Renderer
	{
		struct Descriptor
		{
            /**
             * @brief 윈도우.
             */
            void* window;

			/**
			 * @brief 너비.
			 */
			int width;

			/**
			 * @brief 
			 */
            int height;
		};

		/**
		 * @brief 
		 * @return 
		 */
		[[nodiscard]] RenderDevice* GetDevice();

		/**
		 * @brief 
		 * @param desc
		 */
		void Initialize(const Descriptor& desc);

		/**
		 * @brief 
		 */
		void Release();
	}
}

#endif