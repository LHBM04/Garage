#include "garage/rendering/renderer.hpp"
#include "garage/rendering/render_device.hpp"

#if defined(GARAGE_RENDERER_GL)
    #include "rendering/gl/render_device_gl.hpp"
#endif

#include <memory>

namespace Garage
{
	namespace
	{
		/**
		 * @brief 렌더러 인스턴스.
		 */
		RenderDevice* device = nullptr;
	}

	RenderDevice* Renderer::GetDevice()
    {
        return device;
    }

    void Renderer::Initialize(const Descriptor& desc)
	{
        RenderDevice::Specification spec;
        spec.window = desc.window;
        spec.width = desc.width;
        spec.height = desc.height;

#if defined(GARAGE_RENDERER_GL)
        renderDevice = new RenderDeviceGL;
#endif
        device->Initialize(spec);
    }

    void Renderer::Release()
    {
        if (device)
        {
            device->Release();
            delete device;
            device = nullptr;
        }
	}
}