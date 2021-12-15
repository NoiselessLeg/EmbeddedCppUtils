#pragma once
#include <utility>
#include "ntl_defines.h"

namespace ntl
{
   template <typename T, typename U>
   constexpr bool cmp_equal(T t, U u) noexcept
   {
      using UT = std::make_unsigned_t<T>;
      using UU = std::make_unsigned_t<U>;

      NTL_CONSTEXPR_IF(std::is_signed_v<T> == std::is_signed_v<U>)
      {
         return t == u;
      }
      NTL_CONSTEXPR_ELSEIF(std::is_signed_v<T>)
      {
         return t < 0 ? false : UT(t) == u;
      }
      else
      {
         return u < 0 ? false : t == UU(u);
      }
   }

   template <typename T, typename U>
   constexpr bool cmp_not_equal(T t, U u) noexcept
   {
      return !cmp_equal(t, u);
   }

   template <typename T, typename U>
   constexpr bool cmp_less(T t, U u) noexcept
   {
      using UT = std::make_unsigned_t<T>;
      using UU = std::make_unsigned_t<U>;

      NTL_CONSTEXPR_IF(std::is_signed_v<T> == std::is_signed_v<U>)
      {
         return t < u;
      }
      NTL_CONSTEXPR_ELSEIF(std::is_signed_v<T>)
      {
         return t < 0 ? true : UT(t) < u;
      }
      else
      {
         return u < 0 ? false : t < UU(u);
      }
   }

   template <typename T, typename U>
   constexpr bool cmp_greater(T t, U u) noexcept
   {
      return cmp_less(u, t);
   }

   template <typename T, typename U>
   constexpr bool cmp_less_equal(T t, U u) noexcept
   {
      return !cmp_greater(t, u);
   }

   template <typename T, typename U>
   constexpr bool cmp_greater_equal(T t, U u) noexcept
   {
      return !cmp_less(t, u);
   }
}