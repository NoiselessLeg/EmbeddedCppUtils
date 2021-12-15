#pragma once
#include <algorithm>

namespace ntl
{
   template <typename IterTy, typename Ty>
   IterTy binary_find(IterTy begin, IterTy end, const Ty& value)
   {
      auto result = std::equal_range(begin, end, value);
      return result.first;
   }

   template <typename IterTy, typename Ty, typename CompareTy>
   IterTy binary_find(IterTy begin, IterTy end, const Ty& value, CompareTy comp)
   {
      auto result = std::equal_range(begin, end, value, comp);
      return result.first;
   }
}