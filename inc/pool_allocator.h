// Copyright(c) 2021 John Lewis
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
#include "ntl_defines.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "bounded_queue.h"
#include "Singleton.h"

namespace ntl
{
   template <typename T, std::size_t Capacity, class Allocator = std::allocator<T>>
   class shared_pointer_pool
   {
   public:
      static_assert(!std::is_array_v<T>, "shared_pointer_pool cannot be used with array types.");

      shared_ptr_pool()
      {
         for (std::size_t i = 0; i < Capacity; ++i)
         {
            m_Ptrs.emplace(std::shared_ptr<T>(get_value_as_pointer(i), [&](T* ptr) {
                  std::allocator_traits<Allocator>::destroy(m_Alloc, ptr);
                  m_Ptrs.emplace()
               })
            );
         }
      }

   private:
      const T* get_value_as_pointer(std::size_t idx) const
      {
         return reinterpret_cast<T*>(&m_Data[i]);
      }

      T* get_value_as_pointer(std::size_t idx) const
      {
         return reinterpret_cast<T*>(&m_Data[i]);
      }

      using data_array = std::array<std::aligned_storage_t<sizeof(T), alignof(T)>, Capacity>;
      using ptr_array = ntl::bounded_queue<T, Capacity>;

      std::mutex m_QMtx;
      data_array m_Data;
      ptr_array m_Ptrs;
      Allocator m_Alloc;
   };


   template <typename T, std::size_t Capacity, typename ... Args>
   std::shared_ptr<T> allocate_shared_from_pool(Args&& ... args)
   {
      auto& pool_ref = ntl::Singleton<shared_pointer_pool<T, Capacity>>::instance();
      return std::allocate_shared<T>(pool_ref, std::forward<Args>(args)...);
   }
}