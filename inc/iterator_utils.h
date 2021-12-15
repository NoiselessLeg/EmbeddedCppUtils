#pragma once
#include <map>

namespace ntl
{
   template <typename InIter, typename OutIter, typename Pred>
   void select(InIter begin, InIter end, OutIter insertItr, Pred func)
   {
      while (begin != end)
      {
         if (func(*begin))
         {
            *insertItr = *begin;
            ++insertItr;
         }

         ++begin;
      }
   }

   template <typename KeyType, typename ValType, typename ExecFunc>
   void find_and_execute_fcn(const std::map<KeyType, ValType>& map,
                             const KeyType& elem,
                             ExecFunc func)
   {
      auto iter = map.find(elem);
      if (iter != map.end())
      {
         func(iter);
      }
   }

   template <typename IterType, typename KeyFunc, typename ExecFunc>
   void find_and_execute_fcn(IterType begin,
                             IterType end,
                             KeyFunc keyFunc,
                             ExecFunc func)
   {
      auto iter = std::find_if(begin, end, keyFunc);
      if (iter != end)
      {
         func(iter);
      }
   }
}