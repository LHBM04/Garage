/**
 * @file renderer.hpp
 */
#ifndef __GARAGE_RENDERER_HPP__
#define __GARAGE_RENDERER_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

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
        [[nodiscard]] RenderDevice* GARAGE_API GetDevice();

		/**
		 * @brief 
		 * @param desc
		 */
        void GARAGE_API Initialize(const Descriptor& desc);

		/**
		 * @brief 
		 */
        void GARAGE_API Release();
	}
}

#endif