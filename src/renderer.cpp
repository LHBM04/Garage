#include "../garage/graphics/renderer.hpp"
#include "../garage/graphics/renderer_base.hpp"
#include "../garage/graphics/gl/renderer_gl.hpp"

#include <memory>

namespace Garage
{
	namespace
	{
		/**
		 * @brief 렌더러 인스턴스.
		 */
		RendererBase* handle;
	}

	void Renderer::Initialize(const Descriptor& desc)
	{
        RendererBase::Specification spec;
        spec.window = desc.window;
        spec.width = desc.width;
        spec.height = desc.height;

        handle = new RendererGL;
        handle->Initialize(spec);
    }

    void Renderer::Release()
    {
        if (handle)
        {
            handle->Release();
            delete handle;
        }
	}
}