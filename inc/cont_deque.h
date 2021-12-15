#pragma once
#include <memory>
#include <vector>

namespace ntl
{
   template <
             typename T,
             typename Allocator = std::allocator<T>,
             typename UnderlyingT = std::vector<T, Allocator>
            >
   class contiguous_deque
   {
      using iterator = typename UnderlyingT::iterator;
      using const_iterator = typename UnderlyingT::const_iterator;
      using reference = typename UnderlyingT::reference;


   private:
      UnderlyingT m_Underlying;
      const_iterator m_Begin;

   };
}