#pragma once

namespace ntl
{
   template <class IterTy>
   class circular_iterator
   {
   public:
      using iterator_category = typename IterTy::iterator_category;
      using value_type = typename IterTy::value_type;
      using difference_type = typename IterTy::difference_type;
      using pointer = typename IterTy::pointer;
      using reference = typename IterTy::reference;

      circular_iterator(const IterTy& begin, const IterTy& end) :
         m_Begin(begin),
         m_End(end),
         m_Elem(m_Begin)
      {
      }

      circular_iterator(const circular_iterator& rhs) = default;
      circular_iterator(circular_iterator&& rhs) = default;

      circular_iterator& operator =(const circular_iterator& rhs) = default;
      circular_iterator& operator =(circular_iterator&& rhs) = default;

      ~circular_iterator() = default;

      

   private:
      IterTy m_Begin;
      IterTy m_End;
      IterTy m_Elem;
   };
}