// Copyright(c) 2020 John Lewis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>

#include "ntl_defines.h"

namespace ntl
{
   template <typename ElemTy, 
             std::size_t MaxElemCt, 
             typename Container = std::array<std::aligned_storage_t<sizeof(ElemTy), alignof(ElemTy)>, MaxElemCt>,
             typename Allocator = std::allocator<ElemTy>>
   class bounded_stack
   {
   public:

      using container_type = Container;
      using value_type = typename Container::value_type;
      using size_type = std::size_t;
      using reference = typename Container::reference;
      using const_reference = typename Container::const_reference;
      using allocator_type = Allocator;

      bounded_stack() :
         m_Elems({}),
         m_StackItr(m_Elems.begin())
      {
      }

      explicit bounded_stack(const Container& cont) :
         m_Elems(cont),
         m_StackItr(m_Elems.begin())
      {
      }

      explicit bounded_stack(Container&& cont) :
         m_Elems(std::move(cont)),
         m_StackItr(m_Elems.begin())
      {
      }

      bounded_stack(const bounded_stack& other) :
         m_Elems(other.m_Elems),
         m_StackItr(m_Elems.begin())
      {
         size_type ptrDelta = std::distance(other.m_Elems.begin(), other.m_StackItr);
         std::advance(m_StackItr, tailDelta);
      }

      bounded_stack(bounded_stack&& other) :
      {
         // need to know this before we potentially invalidate other.m_Elems
         size_type headDelta = std::distance(other.m_Elems.begin(), other.m_StackItr);
         m_Elems = std::move(other.m_Elems);
         m_StackItr = m_Elems.begin();
         std::advance(m_StackItr, headDelta);
      }

      bounded_stack& operator = (const bounded_stack& rhs)
      {
         if (this != &rhs)
         {
            m_Elems = rhs.m_Elems;

            m_StackItr = m_Elems.begin();
            size_type headDelta = std::distance(other.m_Elems.begin(), other.m_StackItr);
            std::advance(m_StackItr, headDelta);
         }

         return *this;
      }

      bounded_stack& operator = (bounded_stack&& rhs)
      {
         if (this != &rhs)
         {
            // need to know this before we potentially invalidate other.m_Elems
            size_type headDelta = std::distance(other.m_Elems.begin(), other.m_StackItr);

            m_Elems = std::move(rhs.m_Elems);
            m_StackItr = m_Elems.begin();
            std::advance(m_StackItr, headDelta);
         }

         return *this;
      }

      constexpr size_type capacity() const noexcept
      {
         return m_Elems.max_size();
      }

      template <typename ... Args>
      void emplace(Args&& ... args)
      {
         if (is_full())
         {
            NTL_THROW(std::runtime_error("Stack is full - cannot emplace"));
         }

         std::allocator_traits<allocator_type>::construct(m_Allocator, &(*m_StackItr), std::forward<Args>(args)...);
         ++m_StackItr;
      }

      bool empty() const noexcept
      {
         return m_StackItr == m_Elems.begin();
      }

      allocator_type get_allocator() const noexcept
      {
         return m_Allocator;
      }

      bool is_full() const noexcept
      {
         return m_StackItr == m_Elems.end();
      }

      constexpr size_type max_size() const noexcept
      {
         return capacity();
      }

      void pop()
      {
         if (!empty())
         {
            std::allocator_traits<allocator_type>::destroy(m_Allocator, &(*m_StackItr));
            --m_StackItr;
         }
      }

      void push(const ElemTy& newVal)
      {
         if (is_full())
         {
            NTL_THROW(std::runtime_error("Stack is full - cannot push"));
         }

         std::allocator_traits<allocator_type>::construct(m_Allocator, &(*m_StackItr), newVal);
         ++m_StackItr;
      }

      void push(ElemTy&& newVal)
      {
         if (is_full())
         {
            NTL_THROW(std::runtime_error("Stack is full - cannot push"));
         }

         std::allocator_traits<allocator_type>::construct(m_Allocator, &(*m_StackItr), std::move(newVal));
         ++m_StackItr;
      }

      size_type size() const noexcept
      {
         return std::distance(m_Elems.begin(), m_StackItr)
      }

      const_reference top() const noexcept
      {
         return *m_StackItr;
      }

      reference top() noexcept
      {
         return *m_StackItr;
      }

   private:
      using iterator = typename container_type::iterator;

      allocator_type m_Allocator;
      container_type m_Elems;
      iterator m_StackItr;
   };
}