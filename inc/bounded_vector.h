#pragma once
#include <cassert>
#include <iterator>
#include <memory>
#include <stdexcept>

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

      class reverse_iterator
      {
      public:
         using iterator_category = std::random_access_iterator_tag;
         using value_type = T;
         using difference_type = typename bounded_vector::difference_type;
         using pointer = typename bounded_vector::pointer;
         using reference = typename bounded_vector::reference;

         reverse_iterator() :
            m_Ptr(nullptr)
         {
         }

         reverse_iterator(pointer ptr) :
            m_Ptr(ptr)
         {
         }

         reverse_iterator(const reverse_iterator& rhs) = default;
         reverse_iterator(reverse_iterator&& rhs) = default;

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
            return m_Ptr < rhs.m_Ptr;
         }

         template <typename Other>
         bool operator >= (const Other& rhs) const
         {
            return (*this > rhs) || (*this == rhs);
         }

         template <typename Other>
         bool operator < (const Other& rhs) const
         {
            return m_Ptr > rhs.m_Ptr;
         }

         template <typename Other>
         bool operator <= (const Other& rhs) const
         {
            return (*this < rhs) || (*this == rhs);
         }

         reverse_iterator& operator ++()
         {
            --m_Ptr;
            return *this;
         }

         reverse_iterator operator++(int unused)
         {
            reverse_iterator i = *this;
            --m_Ptr;
            return i;
         }

         reverse_iterator& operator --()
         {
            ++m_Ptr;
            return *this;
         }

         reverse_iterator operator--(int unused)
         {
            reverse_iterator i = *this;
            ++m_Ptr;
            return i;
         }

         reverse_iterator& operator += (difference_type n)
         {
            m_Ptr -= n;
            return *this;
         }

         reverse_iterator operator + (difference_type n) const
         {
            reverse_iterator newItr(m_Ptr - n);
            return newItr;
         }

         reverse_iterator& operator -= (difference_type n)
         {
            m_Ptr += n;
            return *this;
         }

         reverse_iterator operator - (difference_type n) const
         {
            reverse_iterator ret(m_Ptr + n);
            return ret;
         }

         difference_type operator - (const reverse_iterator& rhs) const
         {
            return m_Ptr + rhs.m_Ptr;
         }

         reference operator [](size_type n)
         {
            return *(m_Ptr - n);
         }

         const_reference operator [](size_type n) const
         {
            return *(m_Ptr - n);
         }


      private:
         friend class const_reverse_iterator;
         pointer m_Ptr;
      };

      class const_reverse_iterator
      {
      public:
         using iterator_category = std::random_access_iterator_tag;
         using value_type = T;
         using difference_type = typename bounded_vector::difference_type;
         using pointer = typename bounded_vector::const_pointer;
         using reference = typename bounded_vector::const_reference;

         const_reverse_iterator() :
            m_Ptr(nullptr)
         {
         }

         const_reverse_iterator(pointer ptr) :
            m_Ptr(ptr)
         {
         }

         const_reverse_iterator(const const_reverse_iterator& rhs) = default;
         const_reverse_iterator(const_reverse_iterator&& rhs) = default;

         const_reverse_iterator(const reverse_iterator& rhs) :
            m_Ptr(rhs.m_Ptr)
         {
         }

         const_reverse_iterator& operator = (const reverse_iterator& rhs)
         {
            m_Ptr = rhs.m_Ptr;
            return *this;
         }

         const_reverse_iterator(reverse_iterator && rhs) :
            m_Ptr(std::move(rhs.m_Ptr))
         {
         }

         const_reverse_iterator& operator = (reverse_iterator&& rhs)
         {
            m_Ptr = std::move(rhs.m_Ptr);
            return *this;
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
            return m_Ptr < rhs.m_Ptr;
         }

         template <typename Other>
         bool operator >= (const Other& rhs) const
         {
            return (*this > rhs) || (*this == rhs);
         }

         template <typename Other>
         bool operator < (const Other& rhs) const
         {
            return m_Ptr > rhs.m_Ptr;
         }

         template <typename Other>
         bool operator <= (const Other& rhs) const
         {
            return (*this < rhs) || (*this == rhs);
         }

         const_reverse_iterator& operator ++()
         {
            --m_Ptr;
            return *this;
         }

         const_reverse_iterator operator++(int unused)
         {
            const_reverse_iterator i = *this;
            --m_Ptr;
            return i;
         }

         const_reverse_iterator& operator --()
         {
            ++m_Ptr;
            return *this;
         }

         const_reverse_iterator operator--(int unused)
         {
            const_reverse_iterator i = *this;
            ++m_Ptr;
            return i;
         }

         const_reverse_iterator& operator += (difference_type n)
         {
            m_Ptr -= n;
            return *this;
         }

         const_reverse_iterator operator + (difference_type n) const
         {
            const_reverse_iterator newItr(m_Ptr - n);
            return newItr;
         }

         const_reverse_iterator& operator -= (difference_type n)
         {
            m_Ptr += n;
            return *this;
         }

         const_reverse_iterator operator - (difference_type n) const
         {
            const_reverse_iterator ret(m_Ptr + n);
            return ret;
         }

         difference_type operator - (const const_reverse_iterator& rhs) const
         {
            return m_Ptr + rhs.m_Ptr;
         }

         const_reference operator [](size_type n) const
         {
            return *(m_Ptr - n);
         }
      private:
         friend class reverse_iterator;
         pointer m_Ptr;
      };

      bounded_vector() noexcept:
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
            throw std::out_of_range();
         }

         return *get_element_as_pointer(pos);
      }

      const_reference at(size_type pos) const
      {
         if (pos > size())
         {
            throw std::out_of_range();
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

      void clear() noexcept
      {
         reset();
      }

      constexpr size_type capacity() const noexcept
      {
         return MaxElems;
      }

      constexpr size_type max_size() const noexcept
      {
         return capacity();
      }

      allocator_type get_allocator() const noexcept
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

      const_reference back() const
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
            *m_LastElem = elem;
            ++m_LastElem;
         }
         else
         {
            throw std::runtime_error("No space available to push_back");
         }
      }

      void push_back(T&& elem)
      {
         if (size() < capacity())
         {
            *m_LastElem = std::move(elem);
            ++m_LastElem;
         }
         else
         {
            throw std::runtime_error("No space available to push_back");
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
            throw std::runtime_error("No space available to emplace_back");
         }
      }

      void pop_back()
      {
         --m_LastElem;
         std::allocator_traits<allocator_type>::destroy(m_Alloc, m_LastElem);
      }

      bool empty() const noexcept
      {
         return begin() == end();
      }

      iterator insert(const_iterator pos, const T& value)
      {
         if (size() < capacity())
         {
            iterator currLastElem = end();
            iterator next = end();
            --next;

            while (next != pos)
            {
               std::swap(currLastElem, next);
               --currLastElem;
               --next;
            }

            ++m_LastElem;

            *next = value;
            return next;
         }
         else
         {
            throw std::runtime_error("No space available to insert");
         }
      }

      iterator insert(const_iterator pos, T&& value)
      {
         if (size() < capacity())
         {
            iterator currLastElem = end();
            iterator next = end();
            --next;

            while (next != pos)
            {
               std::swap(currLastElem, next);
               --currLastElem;
               --next;
            }

            ++m_LastElem;

            *next = std::move(value);
            return next;
         }
         else
         {
            throw std::runtime_error("No space available to insert");
         }
      }

      template <typename ... Args>
      iterator emplace(const_iterator pos, Args&&... args)
      {
         if (size() < capacity())
         {
            iterator currLastElem = end();
            iterator next = end();
            --next;

            while (next != pos)
            {
               std::iter_swap(currLastElem, next);
               --currLastElem;
               --next;
            }

            std::iter_swap(currLastElem, next);
            ++m_LastElem;

            std::allocator_traits<allocator_type>::construct(m_Alloc, &(*next), std::forward<Args>(args)...);
            return next;
         }
         else
         {
            throw std::runtime_error("No space available to insert");
         }
      }

      iterator erase(const_iterator pos)
      {
         assert(pos != end());

         const_iterator target = pos;

         T* elem = &(const_cast<T&>(*target));
         iterator mutableTarget(elem);
         iterator next(mutableTarget);
         iterator ret = next;
         ++next;

         while (next != end())
         {
            std::iter_swap(mutableTarget, next);
            ++mutableTarget;
            ++next;
         }

         std::iter_swap(mutableTarget, next);
         pop_back();

         return ret;
      }

      iterator erase(const_iterator first, const_iterator last)
      {
         iterator ret = last;
         if (first != last)
         {
            assert(first != end());
            assert(last != end());

            auto numElemsInRange = std::distance(first, last);
            iterator next = last;
            ++next;
            while (next != end())
            {
               std::iter_swap(first, next);
               ++first;
               ++next;
            }

            for (auto i = 0; i < numElemsInRange; ++i)
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

      allocator_type m_Alloc;
      pointer m_LastElem;

      std::aligned_storage_t<sizeof(T), alignof(T)> m_Elems[MaxElems];
   };
}
