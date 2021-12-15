#pragma once
#include <algorithm>
#include <cctype>
#include <string>

namespace ntl
{
   template <typename StrTy>
   StrTy tolower(const StrTy& str)
   {
      StrTy outputData = str;
      std::transform(str.begin(), str.end(), outputData.begin(), [](typename StrTy::value_type c) { return std::tolower(c); });
      return outputData;
   }
}