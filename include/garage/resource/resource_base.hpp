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

		virtual bool Load(Path& path) = 0;
	};
}

#endif