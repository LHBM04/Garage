#include "garage/rendering/renderer.hpp"
#include "garage/rendering/render_device.hpp"
#include "rendering/gl/render_device_gl.hpp"

#include <memory>

namespace Garage
{
	namespace
	{
		/**
		 * @brief 렌더러 인스턴스.
		 */
		RenderDevice* renderDevice = nullptr;
	}

	void Renderer::Initialize(const Descriptor& desc)
	{
        RenderDevice::Specification spec;
        spec.window = desc.window;
        spec.width = desc.width;
        spec.height = desc.height;

        renderDevice = new RenderDeviceGL;
        renderDevice->Initialize(spec);
    }

    void Renderer::Release()
    {
        if (renderDevice)
        {
            renderDevice->Release();
            delete renderDevice;
            renderDevice = nullptr;
        }
	}
}