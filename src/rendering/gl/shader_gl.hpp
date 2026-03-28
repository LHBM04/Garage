#ifndef __GARAGE_SHADERGL_HPP__
#define __GARAGE_SHADERGL_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/rendering/shader.hpp"

namespace Garage
{
	/**
	 * @class ShaderGL
	 * 
	 * @brief GL 셰이더를 정의합니다.
	 */
	class GARAGE_API ShaderGL : public Shader
	{
	public:
		~ShaderGL() override = default;

		/**
		 * @brief
		 * @param name
		 * @param value
		 */
        void SetUniformInt(Name name, int value) override;

		/**
		 * @brief
		 * @param name
		 * @param value
		 */
        void SetUniformFloat(Name name, float value) override;

		/**
		 * @brief
		 * @param name
		 * @param value
		 */
        void SetUniformVector2(Name name, void* value) override;

		/**
		 * @brief
		 * @param name
		 * @param value
		 */
        void SetUniformVector3(Name name, void* value) override;

		/**
		 * @brief
		 * @param name
		 * @param value
		 */
        void SetUniformVector4(Name name, void* value) override;

		/**
		 * @brief
		 * @param name
		 * @param value
		 */
        void SetUniformMatrix4x4(Name name, void* value) override;

		/**
		 * @brief 
		 * @param path 
		 * @return 
		 */
		bool Load(Path& path) override;

		/**
		 * @brief
		 */
        UInt32 Compile(EType type, Source fragmentSource) override;

		/**
		 * @brief
		 */
		void Use() override;
	};
}

#endif