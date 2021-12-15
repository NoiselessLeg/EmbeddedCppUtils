#pragma once
#include <array>
#include <cstddef>
#include <string>

namespace ntl
{
   template <std::size_t Capacity, class CharT, class Traits = std::char_traits<CharT>>
   class basic_bounded_string
   {
   public:
      using traits_type = Traits;
      using value_type = CharT;
      using size_type = std::size_t;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using iterator = typename std::array<value_type, Capacity>::iterator;
      using const_iterator = typename std::array<value_type, Capacity>::const_iterator;
      using reverse_iterator = typename std::array<value_type, Capacity>::reverse_iterator;
      using const_reverse_iterator = typename std::array<value_type, Capacity>::const_reverse_iterator;

      static constexpr size_type npos = -1;

      constexpr basic_bounded_string() noexcept
      {
         memset(m_DataArray.data(), static_cast<CharT>(0), Capacity * sizeof(CharT));
      }

      constexpr void clear() noexcept
      {
         m_DataArray[0] = static_cast<value_type>(0);
      }

      size_type find(const basic_bounded_string& str, size_type pos = 0) const noexcept
      {
         size_type haystackItr = pos;
         size_type needleItr = 0;


      }

   private:


      std::array<CharT, Capacity> m_DataArray;
   };

   template <std::size_t Capacity>
   using bounded_string = basic_bounded_string<Capacity, char>;
}