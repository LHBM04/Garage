#ifndef GARAGE_RENDERERGL_HPP
#define GARAGE_RENDERERGL_HPP

#include "../garage/graphics/renderer_base.hpp"

namespace Garage
{
	/**
	 * @brief 
	 */
	class RendererGL : public RendererBase
	{
	public:
        ~RendererGL() override;

        /**
         * @brief GL 렌더러를 초기화합니다. 
         */
        bool Initialize(const Specification& specification) override;

        /**
         * @brief 
         */
        void Release() override;

        /**
         * @brief 
         */
        void Clear() override;

        /**
         * @brief 
         */
        void Present() override;
	};
}

#endif