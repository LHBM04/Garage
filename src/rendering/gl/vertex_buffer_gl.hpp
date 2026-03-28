#ifndef __GARAGE_VERTEXBUFFERGL_HPP__
#define __GARAGE_VERTEXBUFFERGL_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/rendering/vertex_buffer.hpp"

namespace Garage
{
	/**
	 * @brief GL 형식의 정점 버퍼를 정의합니다.
	 */
	class GARAGE_API VertexBufferGL : public VertexBuffer
	{
    public:
        /**
         * @brief 
         * @param data 
         * @param size 
         */
        explicit VertexBufferGL(const void* data, UInt32 size);

        /**
         * @brief 소멸자.
         */
        ~VertexBufferGL() override;

        /**
         * @brief 해당 정점 버퍼를 바인딩합니다.
         */
        void Bind() const override;

        /**
         * @brief 해당 정점 버퍼를 언바인딩합니다.
         */
        void Unbind() const override;

    private:
        /**
         * @brief 렌더러의 ID.
         */
        UInt32 rendererID{ 0 };
	};
}  // namespace

#endif