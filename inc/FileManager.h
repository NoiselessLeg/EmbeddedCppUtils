#pragma once
#include <cassert>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "observer_ptr.h"

namespace ntl
{
   template <typename ParsedFileTy>
   class FileLoader
   {
   public:
      template <typename ... Args>
      observer_ptr<ParsedFileTy> load(const std::filesystem::path& filePath, Args&& ... additionalArgs)
      {
         observer_ptr<ParsedFileTy> retPtr;
         std::string filePathStr = filePath.generic_string();

         // do the find here; trying to emplace and checking the iterator result will temporarily
         // construct an object (which will most likely read our file, thus rendering this attempt
         // at stopping multiple redundant file reads moot)
         auto itr = m_FileMap.find(filePathStr);
         if (itr != m_FileMap.end())
         {
            retPtr = itr->second.get();
         }
         else
         {
            auto result = m_FileMap.emplace(filePathStr, 
                                            std::make_unique<ParsedFileTy>(filePathStr, std::forward<Args>(additionalArgs)...));
            assert(result.first);
            retPtr = result.second.get();
         }

         return retPtr;

      }

   private:
      std::unordered_map<std::string, std::unique_ptr<ParsedFileTy>> m_FileMap;

   };
}
