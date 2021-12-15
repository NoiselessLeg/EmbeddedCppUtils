#pragma once
#include <atomic>
#include <cstdint>

namespace ntl
{
   template <class GuidType = std::uint32_t>
   class guid_generator
   {
   public:
      GuidType generate_unique_id() noexcept
      {
         return m_Id++;
      }

   private:
      std::atomic<GuidType> m_Id;
   };
}
