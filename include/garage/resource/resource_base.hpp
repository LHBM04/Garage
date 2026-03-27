#ifndef __GARAGE_RESOURCEBASE_HPP__
#define __GARAGE_RESOURCEBASE_HPP__

#include <filesystem>

namespace Garage
{
	class ResourceBase
	{
		using Path = std::filesystem::path;
	public:
		virtual ~ResourceBase();

		/**
		 * @brief 해당 리소스를 불러옵니다.
		 *
		 * @param path 불러올 리소스의 경로
         *
         * @return bool 해당 리소스의 불러오기 성공 여부
		 */
		virtual bool Load(Path& path) = 0;
	};
}

#endif