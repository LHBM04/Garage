#ifndef __GARAGE_MESHGL_HPP__
#define __GARAGE_MESHGL_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/rendering/mesh.hpp"

namespace Garage
{
    class GARAGE_API MeshGL : public Mesh
	{
	public:
        /**
         * @brief 
         */
        ~MeshGL() override;

        /**
         * @brief 
         * @param path 
         * @return 
         */
        bool Load(Path& path) override;

		/**
		 * @brief 해당 메쉬를 바인딩합니다.
		 */
		void Bind() override;

		/**
		 * @brief 해당 메쉬를 언바인딩합니다.
		 */
		void Unbind() const override;

        /**
         * @brief 해당 메쉬를 그립니다.
         */
        void Draw() const override;
	};
}

#endif