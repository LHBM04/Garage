#ifndef GARAGE_RENDERERGL_HPP
#define GARAGE_RENDERERGL_HPP

#include "garage/rendering/render_device.hpp"

namespace Garage
{
	/**
     * @class RenderDeviceGL
     * 
	 * @brief GL 환경에서의 렌더 디바이스를 정의합니다.
	 */
    class GARAGE_API RenderDeviceGL : public RenderDevice
	{
	public:
        /**
         * @brief 소멸자.
         */
        ~RenderDeviceGL() override;

        /**
         * @brief 해당 렌더 디바이스를 초기화합니다.
         */
        bool Initialize(const Specification& specification) override;

        /**
         * @brief 해당 렌더 디바이스를 해제합니다.
         */
        void Release() override;

        /**
         * @brief 화면을 지웁니다.
         */
        void Clear() override;

        /**
         * @brief 화면을 그립니다.
         */
        void Present() override;
	};
}

#endif