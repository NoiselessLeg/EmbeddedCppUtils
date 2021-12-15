#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace ntl
{
   namespace detail
   {
      template <typename WordType,
                typename = std::enable_if_t<!std::is_same_v<WordType, std::uint16_t> && !std::is_same_v<WordType, std::uint32_t> && !std::is_same_v<WordType, std::uint64_t>>>
      constexpr WordType arbitrary_byteswap(const WordType& word) noexcept
      {
         for (std::size_t i = 0; i < )
      }

      template <typename WordType,
                typename = std::enable_if_t<std::is_same_v<WordType, std::uint16_t>>>
      constexpr std::uint16_t arbitrary_byteswap(const std::uint16_t& word) noexcept
      {
         return _byteswap_ushort(word);
      }

      template <typename WordType,
                typename = std::enable_if_t<std::is_same_v<WordType, std::uint32_t>>>
      constexpr std::uint32_t arbitrary_byteswap(const std::uint32_t& word) noexcept
      {
         return _byteswap_ulong(word);
      }

      template <typename WordType,
                typename = std::enable_if_t<std::is_same_v<WordType, std::uint64_t>>>
      constexpr std::uint64_t arbitrary_byteswap(const std::uint64_t& word) noexcept
      {
         return _byteswap_uint64(word);
      }
   }

   template <typename T, 
             typename WordType,
             typename = std::enable_if_t<std::is_integral_v<WordType>>>
   void structswap(T& obj) noexcept
   {
      static_assert(sizeof(obj) % sizeof(WordType) == 0, "Structure is not multiple of the provided WordSize value.");
      for (std::size_t i = 0; i < sizeof(obj); i += sizeof(WordType))
      {
         WordType wt {};
         memcpy(&wt, &obj, sizeof(wt));
         wt = detail::arbitrary_byteswap(wt);
         memcpy(&obj, &wt, sizeof(wt));
      }
   }
}