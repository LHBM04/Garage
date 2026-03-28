#ifndef __GARAGE_SHADERBASE_HPP__
#define __GARAGE_SHADERBASE_HPP__

#include <cstdint>
#include <string>
#include <string_view>

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/resource/resource.hpp"

namespace Garage
{
	/**
	 * @class ShaderBase
	 * 
	 * @brief 셰이더의 기본 동작을 정의합니다.
	 */
	class GARAGE_API Shader : public Resource
	{
	public:
		/**
		 * @brief 셰이더 이름을 정의합니다.
		 */
		using Name = const std::string&;

		/**
		 * @brief 셰이더 프로그램 ID를 정의합니다.
		 */
		using Handle = std::uint32_t;

		/**
		 * @brief 셰이더 소스를 정의합니다.
		 */
		using Source = const std::string&;

		/**
		 * @enum EType
		 * 
		 * @brief 셰이더의 타입을 정의합니다.
		 */
		enum class EType : UInt8
		{
			/**
			 * @brief 버텍스 셰이더.
			 */
			Vertex,

			/**
			 * @brief 프래그먼트 셰이더.
			 */
			Fragment
		};

		/**
		 * @brief 소멸자.
		 */
		virtual ~Shader() = default;

		/**
		 * @brief 
		 * 
		 * @return 
		 */
		[[nodiscard]] inline Handle GetHandle() const
		{
			return handle;
		}

		/**
		 * @brief 
		 * 
		 * @param name 
		 * @param value 
		 */
		virtual void SetUniformInt(Name name, int value) = 0;

		/**
		 * @brief 
		 * 
		 * @param name 
		 * @param value 
		 */
		virtual void SetUniformFloat(Name name, float value) = 0;

		/**
		 * @brief 
		 * 
		 * @param name 
		 * @param value 
		 */
		virtual void SetUniformVector2(Name name, void* value) = 0;

		/**
		 * @brief 
		 * 
		 * @param name 
		 * @param value 
		 */
		virtual void SetUniformVector3(Name name, void* value) = 0;

		/**
		 * @brief 
		 * 
		 * @param name 
		 * @param value 
		 */
		virtual void SetUniformVector4(Name name, void* value) = 0;

		/**
		 * @brief 
		 * 
		 * @param name 
		 * @param value 
		 */
		virtual void SetUniformMatrix4x4(Name name, void* value) = 0;
		
		/**
		 * @brief 
		 */
		virtual UInt32 Compile(EType type, Source fragmentSource) = 0;

		/**
		 * @brief
		 */
		virtual void Use() = 0;

	protected:
		/**
		 * @brief 
		 */
		Handle handle{ 0 };
	};
}

#endif