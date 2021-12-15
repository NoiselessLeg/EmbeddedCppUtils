#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace ntl
{
   template <std::size_t N>
   class bitset
   {
   public:
      class reference
      {
      public:
         ~reference() noexcept {}

         constexpr reference& operator =(bool x) noexcept
         {
            m_Ref = ((x ? 1 : 0) << m_BitVal);
            return *this;
         }

         constexpr reference& operator =(const reference& x) noexcept
         {
            // determine whether x is true or false, then set
            // our bit to match.
            m_Ref |= ((x ? 1 : 0) << m_BitVal);
            return *this;
         }

         operator bool() const noexcept
         {
            return (m_Ref & (1 << m_BitVal));
         }

         bool operator ~() const noexcept
         {
            uint8_t val = ((m_Ref >> m_BitVal) & 0x1);
            return ~val;
         }

         reference& flip() noexcept
         {
            uint8_t val = ((m_Ref >> m_BitVal) & 0x1);
            val ~= val;
            m_Ref = (val << m_BitVal);
            return *this;
         }

      private:
         friend class bitset<N>;
         reference(uint8_t& ref, uint8_t bit) :
            m_Ref(ref),
            m_BitVal(bit)
         {
            assert(bit < 8);
         }



         uint8_t& m_Ref;
         uint8_t m_BitVal;

      };
   private:
   };
}
