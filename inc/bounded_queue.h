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
#include <cassert>
#include <cstddef>
#include <memory>
#include <stdexcept>

#include "ntl_defines.h"

namespace ntl
{
   template <typename ElemTy, 
             std::size_t MaxElemCt, 
             class Container = std::array<std::aligned_storage_t<sizeof(ElemTy), alignof(ElemTy)>, MaxElemCt>, 
             typename Allocator = std::allocator<ElemTy>>
   class bounded_queue
   {
   public:
      using container_type = Container;
      using value_type = ElemTy;
      using size_type = std::size_t;
      using reference = ElemTy&;
      using const_reference = const ElemTy&;
      using pointer = ElemTy*;
      using const_pointer = const ElemTy*;
      using allocator_type = Allocator;

      bounded_queue() noexcept:
         m_Elems({}),
         m_HeadElemIdx(0),
         m_TailElemIdx(0),
         m_IsFull(false)
      {
      }

      bounded_queue(const bounded_queue& other) noexcept = default;

      bounded_queue(bounded_queue&& other) noexcept = default;

      bounded_queue& operator = (const bounded_queue& rhs) noexcept = default;

      bounded_queue& operator = (bounded_queue&& rhs) noexcept = default;

      ~bounded_queue()
      {
         while (!empty())
         {
            pop();
         }
      }

      const_reference back() const noexcept
      {
         return *get_element_as_pointer(m_TailElemIdx);
      }

      reference back() noexcept
      {
         return *get_element_as_pointer(m_TailElemIdx);
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
            NTL_THROW(std::runtime_error, "Queue is full - cannot emplace");
         }

         std::allocator_traits<allocator_type>::construct(m_Allocator, get_element_as_pointer(m_TailElemIdx), std::forward<Args>(args)...);
         increment_tail();
      }

      bool empty() const noexcept
      {
         return m_TailElemIdx == m_HeadElemIdx && !m_IsFull;
      }

      const_reference front() const noexcept
      {
         return *get_element_as_pointer(m_HeadElemIdx);
      }

      reference front() noexcept
      {
         return *get_element_as_pointer(m_HeadElemIdx);
      }

      allocator_type get_allocator() const noexcept
      {
         return m_Allocator;
      }

      bool is_full() const noexcept
      {
         return m_IsFull;
      }

      constexpr size_type max_size() const noexcept
      {
         return capacity();
      }

      void pop()
      {
         if (!empty())
         {
            std::allocator_traits<allocator_type>::destroy(m_Allocator, get_element_as_pointer(m_HeadElemIdx));
            increment_head();
         }
      }

      void push(ElemTy&& newVal)
      {
         if (is_full())
         {
            NTL_THROW(std::runtime_error, "Queue is full - cannot push");
         }

         std::allocator_traits<allocator_type>::construct(m_Allocator, get_element_as_pointer(m_TailElemIdx), std::forward<ElemTy>(newVal));
         increment_tail();
      }

      size_type size() const noexcept
      {
         if (m_HeadElemIdx < m_TailElemIdx)
         {
            return m_TailElemIdx - m_HeadElemIdx;
         }

         // this is the case where we just started wrapping the queue
         else if (m_HeadElemIdx > m_TailElemIdx)
         {
            return m_TailElemIdx + (m_Elems.max_size() - m_HeadElemIdx);
         }
         else
         {
            return empty() ? 0 : capacity();
         }
      }

   private:
      using index_type = Container::size_type;

      pointer get_element_as_pointer(std::size_t idx) noexcept
      {
         return reinterpret_cast<pointer>(&m_Elems[idx]);
      }

      const_pointer get_element_as_pointer(std::size_t idx) const noexcept
      {
         return reinterpret_cast<const_pointer>(&m_Elems[idx]);
      }

      void increment_head()
      {
         ++m_HeadElemIdx;
         if (m_HeadElemIdx == m_Elems.max_size())
         {
            m_HeadElemIdx = 0;
         }

         m_IsFull = false;

         assert(m_HeadElemIdx < m_Elems.max_size());
      }

      void increment_tail()
      {
         ++m_TailElemIdx;
         if (m_TailElemIdx == m_Elems.max_size())
         {
            m_TailElemIdx = 0;
         }

         if (m_TailElemIdx == m_HeadElemIdx)
         {
            m_IsFull = true;
         }

         assert(m_TailElemIdx < m_Elems.max_size());
      }

      allocator_type m_Allocator;
      container_type m_Elems;
      index_type m_HeadElemIdx;
      index_type m_TailElemIdx;
      bool m_IsFull;
   };
}