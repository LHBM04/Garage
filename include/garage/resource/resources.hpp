#ifndef __GARAGE_RESOURCES_HPP__
#define __GARAGE_RESOURCES_HPP__

#include <unordered_map>

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

namespace Garage
{
	class ResourceManager
	{
	public:
       template <typename TResource>
	   static TResource* LoadResource(const typename TResource::Path& path)
	   {
		   const std::string key = path.string();
		   auto iter = resourceBank.find(key);
		   if (iter != resourceBank.end())
		   {
			   return static_cast<TResource*>(iter->second);
		   }
		   TResource* resource = new TResource;
		   if (resource->Load(path))
		   {
			   resourceBank[key] = resource;
			   return resource;
		   }
		   else
		   {
			   delete resource;
			   return nullptr;
		   }
       }

	   template <typename TResource>
	   static void UnloadResource(const typename TResource::Path& path)
	   {
		   const std::string key = path.string();
		   auto iter = resourceBank.find(key);
		   if (iter != resourceBank.end())
		   {
			   delete iter->second;
			   resourceBank.erase(iter);
		   }
       }

    private:
		/**
		 * @brief 리소스가 저장되는 해쉬 맵을 정의합니다.
		 */
		using ResourceBank = std::unordered_map<std::string, Resource*>;

		/**
		 * @brief 리소스 뱅크.
		 */
		static ResourceBank resourceBank;
	};
}

#endif