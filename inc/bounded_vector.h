// Copyright(c) 2020 John Lewis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
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
#include <iterator>
#include <memory>
#include <stdexcept>

#include "ntl_defines.h"
#include "ntl_exception.h"

namespace ntl
{


   template <typename T, std::size_t MaxElems, typename Allocator = std::allocator<T>>
   class bounded_vector
   {
   public:
      using value_type = T;
      using allocator_type = Allocator;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = typename std::allocator_traits<allocator_type>::pointer;
      using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
      using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;

      class iterator
      {
      public:
         using iterator_category = std::random_access_iterator_tag;
         using value_type = T;
         using difference_type = typename bounded_vector::difference_type;
         using pointer = typename bounded_vector::pointer;
         using reference = typename bounded_vector::reference;

         iterator() :
            m_Ptr(nullptr)
         {
         }

         iterator(pointer ptr) :
            m_Ptr(ptr)
         {
         }

         iterator(const iterator& rhs) = default;
         iterator& operator = (const iterator& rhs) = default;

         iterator(iterator&& rhs) = default;
         iterator& operator = (iterator&& rhs) = default;

         iterator& operator = (pointer ptr)
         {
            m_Ptr = ptr;
            return *this;
         }

         reference operator *()
         {
            return *m_Ptr;
         }

         pointer operator ->()
         {
            return m_Ptr;
         }

         const_reference operator *() const
         {
            return *m_Ptr;
         }

         const_pointer operator ->() const
         {
            return m_Ptr;
         }

         template <typename Other>
         bool operator == (const Other& rhs) const
         {
            return m_Ptr == rhs.m_Ptr;
         }

         template <typename Other>
         bool operator != (const Other& rhs) const
         {
            return !(*this == rhs);
         }

         template <typename Other>
         bool operator > (const Other& rhs) const
         {
            return m_Ptr > rhs.m_Ptr;
         }

         template <typename Other>
         bool operator >= (const Other& rhs) const
         {
            return (*this > rhs) || (*this == rhs);
         }

         template <typename Other>
         bool operator < (const Other& rhs) const
         {
            return !(*this >= rhs);
         }

         template <typename Other>
         bool operator <= (const Other& rhs) const
         {
            return !(*this > rhs);
         }

         iterator& operator ++()
         {
            ++m_Ptr;
            return *this;
         }

         iterator operator++(int unused)
         {
            iterator i = *this;
            ++m_Ptr;
            return i;
         }

         iterator& operator --()
         {
            --m_Ptr;
            return *this;
         }

         iterator operator--(int unused)
         {
            iterator i = *this;
            --m_Ptr;
            return i;
         }

         iterator& operator += (difference_type n)
         {
            m_Ptr += n;
            return *this;
         }

         iterator operator + (difference_type n) const
         {
            iterator newItr(m_Ptr + n);
            return newItr;
         }

         iterator& operator -= (difference_type n)
         {
            m_Ptr -= n;
            return *this;
         }

         iterator operator - (difference_type n) const
         {
            iterator ret(m_Ptr - n);
            return ret;
         }

         difference_type operator - (const iterator& rhs) const
         {
            return m_Ptr - rhs.m_Ptr;
         }

         reference operator [](size_type n)
         {
            return *(m_Ptr + n);
         }

         const_reference operator [](size_type n) const
         {
            return *(m_Ptr + n);
         }

      private:
         friend class const_iterator;
         pointer m_Ptr;
      };

      class const_iterator
      {
      public:
         using iterator_category = std::random_access_iterator_tag;
         using value_type = T;
         using difference_type = typename bounded_vector::difference_type;
         using pointer = typename bounded_vector::const_pointer;
         using reference = typename bounded_vector::const_reference;

         const_iterator() :
            m_Ptr(nullptr)
         {
         }

         const_iterator(pointer ptr) :
            m_Ptr(ptr)
         {
         }

         const_iterator(const const_iterator& rhs) = default;
         const_iterator& operator = (const const_iterator& rhs) = default;

         const_iterator(const_iterator&& rhs) = default;
         const_iterator& operator = (const_iterator&& rhs) = default;

         const_iterator(const iterator& rhs):
            m_Ptr(rhs.m_Ptr)
         {
         }

         const_iterator& operator = (const iterator & rhs)
         {
            m_Ptr = rhs.m_Ptr;
            return *this;
         }

         const_iterator(iterator&& rhs) :
            m_Ptr(std::move(rhs.m_Ptr))
         {
         }

         const_iterator& operator = (iterator&& rhs)
         {
            m_Ptr = std::move(rhs.m_Ptr);
            return *this;
         }

         const_iterator& operator = (pointer ptr)
         {
            m_Ptr = ptr;
            return *this;
         }

         reference operator *() const
         {
            return *m_Ptr;
         }

         pointer operator ->() const
         {
            return m_Ptr;
         }

         template <typename Other>
         bool operator == (const Other& rhs) const
         {
            return m_Ptr == rhs.m_Ptr;
         }

         template <typename Other>
         bool operator != (const Other& rhs) const
         {
            return !(*this == rhs);
         }

         template <typename Other>
         bool operator > (const Other& rhs) const
         {
            return m_Ptr > rhs.m_Ptr;
         }

         template <typename Other>
         bool operator >= (const Other& rhs) const
         {
            return (*this > rhs) || (*this == rhs);
         }

         template <typename Other>
         bool operator < (const Other& rhs) const
         {
            return !(*this >= rhs);
         }

         template <typename Other>
         bool operator <= (const Other& rhs) const
         {
            return !(*this > rhs);
         }

         const_iterator& operator ++()
         {
            ++m_Ptr;
            return *this;
         }

         const_iterator operator++(int unused)
         {
            const_iterator i = *this;
            ++m_Ptr;
            return i;
         }

         const_iterator& operator --()
         {
            --m_Ptr;
            return *this;
         }

         const_iterator operator--(int unused)
         {
            const_iterator i = *this;
            --m_Ptr;
            return i;
         }

         const_iterator& operator += (difference_type n)
         {
            m_Ptr += n;
            return *this;
         }

         const_iterator operator + (difference_type n) const
         {
            const_iterator newItr(m_Ptr + n);
            return newItr;
         }

         const_iterator& operator -= (difference_type n)
         {
            m_Ptr -= n;
            return *this;
         }

         const_iterator operator - (difference_type n) const
         {
            const_iterator ret(m_Ptr - n);
            return ret;
         }

         difference_type operator - (const const_iterator& rhs) const
         {
            return m_Ptr - rhs.m_Ptr;
         }

         const_reference operator [](size_type n) const
         {
            return *(m_Ptr + n);
         }

      private:
         friend class iterator;
         pointer m_Ptr;
      };

      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      bounded_vector() noexcept :
         m_LastElem(get_element_as_pointer(0))
      {
      }

      bounded_vector(const bounded_vector& rhs) noexcept :
         m_Elems(rhs.m_Elems)
      {
         auto numElems = std::distance(rhs.begin(), rhs.end());
         m_LastElem = get_element_as_pointer(0);
         for (auto i = 0; i < numElems; ++i)
         {
            ++m_LastElem;
         }
      }

      bounded_vector(bounded_vector&& rhs) noexcept
      {
         auto numElems = std::distance(rhs.begin(), rhs.end());
         m_LastElem = get_element_as_pointer(0);
         for (auto i = 0; i < numElems; ++i)
         {
            ++m_LastElem;
         }

         m_Elems = std::move(rhs.m_Elems);
      }

      bounded_vector(std::initializer_list<T> list) :
         m_LastElem(get_element_as_pointer(0))
      {
         assert(list.size() <= capacity());
         for (const auto& elem : list)
         {
            push_back(elem);
         }
      }

      bounded_vector& operator = (const bounded_vector& rhs) noexcept
      {
         if (this != &rhs)
         {
            reset();
            std::copy(rhs.begin(), rhs.end(), std::back_inserter(*this));
         }

         return *this;
      }

      bounded_vector& operator = (bounded_vector&& rhs) noexcept
      {
         if (this != &rhs)
         {
            reset();
            for (auto i = 0; i < rhs.size(); ++i)
            {
               ++m_LastElem;
            }

            m_Elems = std::move(rhs.m_Elems);
         }

         return *this;
      }

      bounded_vector& operator =(std::initializer_list<T> list)
      {
         assert(list.size() <= capacity());
         reset();
         for (const auto& elem : list)
         {
            push_back(elem);
         }

         return *this;
      }

      ~bounded_vector()
      {
         reset();
      }

      iterator begin() noexcept
      {
         return iterator(get_element_as_pointer(0));
      }

      const_iterator begin() const noexcept
      {
         return cbegin();
      }

      const_iterator cbegin() const noexcept
      {
         return const_iterator(get_element_as_pointer(0));
      }

      reverse_iterator rbegin() noexcept
      {
         auto endItr = end();
         --endItr;
         return reverse_iterator(endItr);
      }

      const_reverse_iterator rbegin() const noexcept
      {
         return crbegin();
      }

      const_reverse_iterator crbegin() const noexcept
      {
         auto endItr = end();
         --endItr;
         return const_reverse_iterator(endItr);
      }

      iterator end() noexcept
      {
         return iterator(m_LastElem);
      }

      const_iterator end() const noexcept
      {
         return const_iterator(m_LastElem);
      }

      const_iterator cend() const noexcept
      {
         return const_iterator(m_LastElem);
      }

      reverse_iterator rend() noexcept
      {
         auto bItr = begin();
         --bItr;
         return reverse_iterator(bItr);
      }

      const_reverse_iterator rend() const noexcept
      {
         return crend();
      }

      const_reverse_iterator crend() const noexcept
      {
         auto bItr = begin();
         --bItr;
         return const_reverse_iterator(bItr);
      }

      reference at(size_type pos)
      {
         if (pos > size())
         {
            NTL_THROW(std::out_of_range);
         }

         return *get_element_as_pointer(pos);
      }

      const_reference at(size_type pos) const
      {
         if (pos > size())
         {
            NTL_THROW(std::out_of_range);
         }

         return *get_element_as_pointer(pos);
      }

      reference operator [](size_type pos) noexcept
      {
         return *get_element_as_pointer(pos);
      }

      const_reference operator [](size_type pos) const noexcept
      {
         return *get_element_as_pointer(pos);
      }

      void clear()
      {
         reset();
      }

      constexpr size_type capacity() const noexcept
      {
         return MaxElems;
      }

      T* data() noexcept
      {
         return get_element_as_pointer(0);
      }

      const T* data() const noexcept
      {
         return get_element_as_pointer(0);
      }

      constexpr size_type max_size() const noexcept
      {
         return capacity();
      }

      constexpr allocator_type get_allocator() const noexcept
      {
         return m_Alloc;
      }

      reference front() noexcept
      {
         return *get_element_as_pointer(0);
      }

      const_reference front() const noexcept
      {
         return *get_element_as_pointer(0);
      }

      reference back() noexcept
      {
         pointer elem = m_LastElem;
         --elem;
         return *elem;
      }

      const_reference back() const noexcept
      {
         pointer elem = m_LastElem;
         --elem;
         return *elem;
      }

      size_type size() const noexcept
      {
         return end() - begin();
      }

      void push_back(const T& elem)
      {
         if (size() < capacity())
         {
            std::allocator_traits<allocator_type>::construct(m_Alloc, &(*m_LastElem), elem);
            ++m_LastElem;
         }
         else
         {
            NTL_THROW(ntl::out_of_space, "No space available to push_back");
         }
      }

      void push_back(T&& elem)
      {
         if (size() < capacity())
         {
            std::allocator_traits<allocator_type>::construct(m_Alloc, &(*m_LastElem), std::move(elem));
            ++m_LastElem;
         }
         else
         {
            NTL_THROW(ntl::out_of_space, "No space available to push_back");
         }
      }

      template <typename ... Args>
      void emplace_back(Args&&... args)
      {
         if (size() < capacity())
         {
            std::allocator_traits<allocator_type>::construct(m_Alloc, m_LastElem, std::forward<Args>(args)...);
            ++m_LastElem;
         }
         else
         {
            NTL_THROW(ntl::out_of_space, "No space available to emplace_back");
         }
      }

      void pop_back()
      {
         if (end() > begin())
         {
            --m_LastElem;
            std::allocator_traits<allocator_type>::destroy(m_Alloc, m_LastElem);
         }
      }

      bool empty() const noexcept
      {
         return begin() == end();
      }

      iterator insert(const_iterator pos, const T& value)
      {
         if (size() < capacity())
         {
            iterator mutable_pos = to_mutable_iterator(pos);

            iterator ltmp = end();
            iterator rtmp = end();
            --ltmp;

            while (ltmp >= pos)
            {
               std::iter_swap(ltmp, rtmp);
               --ltmp;
               --rtmp;
            }

            ++m_LastElem;
            std::allocator_traits<allocator_type>::construct(m_Alloc, &(*mutable_pos), value);

            return mutable_pos;
         }
         else
         {
            NTL_THROW(ntl::out_of_space, "No space available to insert");
         }
      }

      iterator insert(const_iterator pos, T&& value)
      {
         if (size() < capacity())
         {
            iterator mutable_pos = to_mutable_iterator(pos);

            iterator ltmp = end();
            iterator rtmp = end();
            --ltmp;

            while (ltmp >= pos)
            {
               std::iter_swap(ltmp, rtmp);
               --ltmp;
               --rtmp;
            }

            ++m_LastElem;
            std::allocator_traits<allocator_type>::construct(m_Alloc, &(*mutable_pos), std::move(value));

            return mutable_pos;
         }
         else
         {
            NTL_THROW(ntl::out_of_space, "No space available to insert");
         }
      }

      template <typename ... Args>
      iterator emplace(const_iterator pos, Args&&... args)
      {
         if (size() < capacity())
         {
            iterator mutable_pos = to_mutable_iterator(pos);

            iterator ltmp = end();
            iterator rtmp = end();
            --ltmp;

            while (ltmp >= pos)
            {
               std::iter_swap(ltmp, rtmp);
               --ltmp;
               --rtmp;
            }

            ++m_LastElem;
            std::allocator_traits<allocator_type>::construct(m_Alloc, &(*mutable_pos), std::forward<Args>(args)...);

            return mutable_pos;
         }
         else
         {
            NTL_THROW(ntl::out_of_space, "No space available to insert");
         }
      }

      iterator erase(const_iterator pos)
      {
         assert(pos < end());

         iterator mutable_target = to_mutable_iterator(pos);
         iterator next(mutable_target);
         iterator ret = next;
         ++next;

         // swap the element into the last position, then
         // pop the container back.
         const_iterator last = end();
         while (next != last)
         {
            std::iter_swap(mutable_target, next);
            ++mutable_target;
            ++next;
         }

         pop_back();

         return ret;
      }

      iterator erase(const_iterator first, const_iterator last)
      {
         assert(first <= last);
         iterator mutable_first = to_mutable_iterator(first);
         iterator mutable_last = to_mutable_iterator(last);
         iterator ret = mutable_first;

         if ((last - first) > 0)
         {
            // first, shift all the elements in the range to the end
            while (mutable_last != end())
            {
               std::iter_swap(mutable_first, mutable_last);
               ++mutable_last;
               ++mutable_first;
            }

            while (end() != mutable_first)
            {
               pop_back();
            }
         }

         return ret;
      }

      bool operator == (const bounded_vector& rhs) const noexcept
      {
         bool isEqual = true;
         if (size() == rhs.size())
         {
            auto thisItr = begin();
            auto rhsItr = rhs.begin();

            while (thisItr != end()
               && rhsItr != rhs.end())
            {
               if (*thisItr != *rhsItr)
               {
                  isEqual = false;
                  break;
               }

               ++thisItr;
               ++rhsItr;
            }
         }
         else
         {
            isEqual = false;
         }

         return isEqual;
      }

      bool operator != (const bounded_vector& rhs) const noexcept
      {
         return !(*this == rhs);
      }

   private:

      pointer get_element_as_pointer(std::size_t idx) noexcept
      {
         return reinterpret_cast<pointer>(&m_Elems[idx]);
      }

      const_pointer get_element_as_pointer(std::size_t idx) const noexcept
      {
         return reinterpret_cast<const_pointer>(&m_Elems[idx]);
      }

      void reset()
      {
         while (end() != begin())
         {
            pop_back();
         }
      }

      constexpr iterator to_mutable_iterator(const_iterator itr) noexcept
      {
         iterator beginItr = begin();
         return beginItr + (itr - beginItr);
      }

      allocator_type m_Alloc;
      pointer m_LastElem;

      std::array<storage_type, MaxElems> m_Elems;
   };
}
