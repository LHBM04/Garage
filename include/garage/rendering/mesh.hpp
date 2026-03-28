#ifndef __GARAGE_MESH_HPP__
#define __GARAGE_MESH_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/resource/resource.hpp"

namespace Garage
{
    /**
     * @class Mesh
     * 
     * @brief 메시를 정의합니다.
     */
    class GARAGE_API Mesh : public Resource
    {
    public:
    	/**
    	 * @struct Vertex
    	 *
    	 * @brief 정점 데이터를 정의합니다.
    	 */
    	struct Vertex final
        {
            /**
             * @brief 정점의 위치.
             */
            float position[3];

            /**
             * @brief 정점의 법선 벡터.
             */
            float normal[3];

            /**
             * @brief 정점의 텍스처 좌표.
             */
            float texCoords[2];
        };

        /**
         * @brief 소멸자.
         */
        virtual ~Mesh() = default;

		/**
		 * @brief 해당 메시를 바인딩합니다.
		 */
		virtual void Bind() const = 0;

		/**
		 * @brief 해당 메시를 언바인딩합니다.
		 */
		virtual void Unbind() const = 0;
        
        /**
         * @brief 해당 메시를 그립니다.
         */
        virtual void Draw() const = 0;

	protected:
		/**
		 * @brief 
		 */
		using VertexList = std::vector<Vertex>;

		/**
		 * @brief 
		 */
		using IndexList = std::vector<UInt32>;
    
        /**
         * @brief 해당 메쉬의 정점 배열 객체.
         */
        UInt32 vertexArray{ 0 };
        
        /**
         * @brief 해당 메쉬의 정점 버퍼 객체.
         */
        UInt32 vertexBuffer{ 0 };
        
        /**
         * @brief 해당 메쉬의 요소 버퍼 객체.
         */
        UInt32 elementBuffer{ 0 };

        /**
         * @brief 
         */
        VertexList vertices;

        /**
         * @brief 
         */
        IndexList indices;
    };
}

#endif