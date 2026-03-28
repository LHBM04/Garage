#ifndef __GARAGE_VERTEXBUFFER_HPP__
#define __GARAGE_VERTEXBUFFER_HPP__

namespace Garage
{
	/**
     * @class VertexBuffer
     * 
	 * @brief 정점 버퍼를 정의합니다.
	 */
	class VertexBuffer
	{
    public:
        /**
         * @brief 소멸자.
         */
        virtual ~VertexBuffer() = default;

        /**
         * @brief 해당 정점 버퍼를 바인딩합니다.
         */
        virtual void Bind() const = 0;

        /**
         * @brief 해당 정점 버퍼를 언바인딩합니다.
         */
        virtual void Unbind() const = 0;
	};
}

#endif