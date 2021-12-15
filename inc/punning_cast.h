#pragma once
#include <cstring>
#include <type_traits>

namespace ntl
{

   template <typename ToTy, typename FromTy>
   constexpr ToTy punning_cast(const FromTy elem[]) noexcept
   {
      ToTy val{};
      memcpy(&val, elem, sizeof(val));
      return val;
   }

   template <typename ToTy, typename FromTy, typename = std::enable_if_t<std::is_pointer_v<FromTy>>>
   constexpr ToTy punning_cast(const FromTy* const elem) noexcept
   {
      ToTy val{};
      memcpy(&val, elem, sizeof(val));
      return val;
   }

   template <typename ToTy, typename FromTy, typename = std::enable_if_t<!std::is_pointer_v<FromTy>>>
   constexpr ToTy punning_cast(const FromTy& elem) noexcept
   {
      ToTy val{};
      static_assert(sizeof(elem) == sizeof(val), "Sizes must be equivalent");
      memcpy(&val, &elem, sizeof(val));
      return val;
   }
}