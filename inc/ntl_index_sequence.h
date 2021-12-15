#pragma once
#include "ntl_defines.h"

#ifndef NTL_FEATURE_HAS_INDEX_SEQUENCE
namespace ntl
{
   template <class T, T... Ints>
   class integer_sequence
   {
      using value_type = T;
      static constexpr std::size_t size() noexcept
      {
         return sizeof...(Ints);
      }
   };

   template <std::size_t ...>
   struct index_sequence {};

   template <std::size_t ... Next>
   struct index_sequence_helper<0U, Next ...>
   {
      using type = index_sequence<Next ...>;
   };


   template <std::size_t N, std::size_t ... Next>
   struct index_sequence_helper : public index_sequence_helper<N - 1U, N - 1U, Next...>
   {
   };


   template <std::size_t ... Ints>
   using index_sequence = integer_sequence<std::size_t, Ints...>;

   template <std::size_t N>
   using make_index_sequence = typename index_sequence_helper<N>::type;
}
#else
#include <utility>
namespace ntl
{
   template <std::size_t... Ints>
   using index_sequence = std::index_sequence<Ints>;
}
#endif