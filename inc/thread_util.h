#pragma once
#include <shared_mutex>
#include <string_view>
#include <thread>
#include <unordered_map>

namespace ntl
{

   class thread_util
   {
   public:
      static const char* get_thread_name(const std::thread::id& thread_id);

      static size_t get_unique_thread_id(const std::thread::id& id);

      static void set_thread_name(const std::thread::id& thread_id, std::string_view thread_name);

   private:
      static std::shared_mutex m_DataMtx;
      static std::unordered_map<size_t, std::string> m_NameMap;

   };


}
