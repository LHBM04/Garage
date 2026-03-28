#ifndef __GARAGE_TEXTUREGL_HPP__
#define __GARAGE_TEXTUREGL_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/rendering/texture.hpp"

namespace Garage
{
	class GARAGE_API TextureGL : public Texture
	{
	public:
		/**
		 * @brief 
		 */
		~TextureGL() override;

		/**
		 * @brief 
		 * 
		 * @param path 
		 * 
		 * @return 
		 */
		bool Load(Path& path) override;

		/**
		 * @brief 
		 */
		void Bind() const override;

		/**
		 * @brief 
		 */
		void Unbind() const override;
    };
}

#endif