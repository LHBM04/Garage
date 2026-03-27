#ifndef __GARAGE_SHADERGL_HPP__
#define __GARAGE_SHADERGL_HPP__

#include "../../../resource/shader_base.hpp"

namespace Garage
{
	/**
	 * @class ShaderGL
	 * 
	 * @brief GL 셰이더를 정의합니다.
	 */
	class ShaderGL : public ShaderBase
	{
    public:
        ~ShaderGL() override = default;

		  /**
         * @brief
         * @return
         */
        Handle GetProgramID() const override;

        /**
         * @brief
         * @param name
         * @param value
         */
        void SetUniformInt(ShaderName name, int value) override;

        /**
         * @brief
         * @param name
         * @param value
         */
        void SetUniformFloat(ShaderName name, float value) override;

        /**
         * @brief
         * @param name
         * @param value
         */
        void SetUniformVector2(ShaderName name, void* value) override;

        /**
         * @brief
         * @param name
         * @param value
         */
        void SetUniformVector3(ShaderName name, void* value) override;

        /**
         * @brief
         * @param name
         * @param value
         */
        void SetUniformVector4(ShaderName name, void* value) override;

        /**
         * @brief
         * @param name
         * @param value
         */
        void SetUniformMatrix4x4(ShaderName name, void* value) override;

        /**
         * @brief
         */
        void Compile(const std::string& vertexSource, const std::string& fragmentSource) override;

        /**
         * @brief
         */
        void Use() override;
	};
}

#endif