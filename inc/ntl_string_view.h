#pragma once
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>

namespace ntl
{
   namespace detail
   {

   }

   template <typename CharT, typename Traits = std::char_traits<CharT>>
   class basic_string_view
   {
   public:
      using traits_type = Traits;
      using value_type = CharT;
      using pointer = CharT*;
      using const_pointer = const CharT*;
      using reference = CharT&;
      using const_reference = const CharT&;

      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;

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
         const CharT* m_Ptr;
      };

      using iterator = const_iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      static constexpr size_type npos = size_type(-1);

      constexpr basic_string_view() noexcept :
         m_Ptr(nullptr),
         m_Size(0)
      {
      }

      constexpr basic_string_view(const basic_string_view& other) noexcept = default;

      constexpr basic_string_view(const CharT* s, size_type count) :
         m_Ptr(s),
         m_Size(count)
      {
      }

      constexpr basic_string_view(const CharT* s) :
         m_Ptr(s),
         m_Size(Traits::length(s))
      {
      }

      constexpr basic_string_view& operator =(const basic_string_view& view) noexcept = default;

      constexpr const_iterator begin() const noexcept
      {
         return const_iterator(m_Ptr);
      }

      constexpr const_iterator cbegin() const noexcept
      {
         return const_iterator(m_Ptr);
      }

      constexpr const_iterator end() const noexcept
      {
         return const_iterator(std::ptrdiff_t(m_Ptr + m_Size));
      }

      constexpr const_iterator cend() const noexcept
      {
         return const_iterator(std::ptrdiff_t(m_Ptr + m_Size));
      }

      constexpr const_reverse_iterator rbegin() const noexcept
      {
         return const_iterator(m_Ptr);
      }

      constexpr const_reverse_iterator crbegin() const noexcept
      {
         return const_iterator(m_Ptr);
      }

      constexpr const_reverse_iterator rend() const noexcept
      {
         return const_iterator(std::ptrdiff_t(m_Ptr + m_Size));
      }

      constexpr const_reverse_iterator crend() const noexcept
      {
         return const_iterator(std::ptrdiff_t(m_Ptr + m_Size));
      }

      constexpr const_reference operator [](size_type pos) const noexcept
      {
         return *(m_Ptr + pos);
      }

      constexpr const_reference at(size_type pos) const
      {
         if (pos >= m_Size)
         {
            NTL_THROW_NOARG(std::out_of_range);
         }

         return *(m_Ptr + m_Size);
      }

      constexpr const_reference front() const noexcept
      {
         return operator[](0);
      }

      constexpr const_reference back() const noexcept
      {
         return operator[](size() - 1);
      }

      constexpr const_pointer data() const noexcept
      {
         return m_Ptr;
      }

      constexpr size_type size() const noexcept
      {
         return m_Size;
      }

      constexpr size_type length() const noexcept
      {
         return m_Size;
      }

      constexpr bool empty() const noexcept
      {
         return m_Size == 0;
      }

      constexpr void remove_prefix(size_type n)
      {
         m_Ptr += n;
      }

      constexpr void remove_suffix(size_type n)
      {
         m_Size -= n;
      }

      constexpr void swap(basic_string_view& v) noexcept
      {
         std::swap(m_Ptr, v.m_Ptr);
         std::swap(m_Size, v.m_Size);
      }

      constexpr size_type copy(CharT* dest, size_type count, size_type  pos = 0) const
      {
         Traits::copy(dest, data() + pos, rcount);
      }

      constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const
      {
         if (pos > size())
         {
            NTL_THROW_NOARG(std::out_of_range);
         }

         return basic_string_view(m_Ptr + pos, std::min(count, size() - pos));
      }

      constexpr int compare(basic_string_view v) const noexcept
      {
         int finalRet = 0;
         int rlen = std::min(size(), v.size());
         int cmpRet = Traits::compare(data(), v.data(), rlen);
         if (cmpRet < 0)
         {
            finalRet = -1;
         }
         else if (cmpRet == 0)
         {
            if (size() < v.size())
            {
               finalRet = -1;
            }
            else if (size == v.size())
            {
               finalRet = 0;
            }
            else
            {
               finalRet = 1;
            }
         }
         else
         {
            finalRet = 1;
         }

         return finalRet;
      }

      constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const
      {
         return substr(pos1, count1).compare(v);
      }

      constexpr int compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const
      {
         return substr(pos1, count1).compare(v.substr(pos2, count2));
      }

      constexpr int compare(const CharT* s) const
      {
         return compare(basic_string_view(s));
      }

      constexpr int compare(size_type pos1, size_type count1, const CharT* s) const
      {
         return substr(pos1, count1).compare(basic_string_view(s));
      }

      constexpr int compare(size_type pos1, size_type count1, const CharT* s, size_type count2) const
      {
         return substr(pos1, count1).compare(basic_string_view(s, count2));
      }

      constexpr bool starts_with(basic_string_view sv) const noexcept
      {
         basic_string_view truncated = substr(0, sv.size());
         return truncated == sv;
      }

      constexpr bool starts_with(CharT c) const noexcept
      {
         bool ret = false;
         if (!empty())
         {
            ret = Traits::eq(front(), c);
         }

         return ret;
      }

      constexpr bool starts_with(const CharT* c) const
      {
         return starts_with(basic_string_view(c));
      }

      constexpr bool ends_with(basic_string_view sv) const noexcept
      {
         bool ret = false;
         if (size() >= sv.size())
         {
            ret = compare(size() - sv.size(), npos, sv) == 0;
         }

         return ret;
      }

      constexpr bool ends_with(CharT c) const noexcept
      {
         bool ret = false;
         if (!empty())
         {
            ret = Traits::eq(back(), c);
         }

         return ret;
      }

      constexpr bool ends_with(const CharT* c) const
      {
         return ends_with(basic_string_view(c));
      }

      constexpr bool contains(basic_string_view sv) const noexcept
      {
         return find(sv.m_Ptr) != npos;
      }

      constexpr bool contains(CharT c) const noexcept
      {
         return find(c) != npos;
      }

      constexpr bool contains(const CharT* s) const
      {
         return find(s) != npos;
      }

      constexpr size_type find(const CharT* c, size_type pos = 0) const
      {

      }

      constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept
      {
         size_type strsize = v.size();
         if (strsize == 0)
         {
            return pos <= strsize ? pos : npos;
         }
         if (pos >= strsize)
         {
            return npos;
         }

         const 
      }

      constexpr size_type find(CharT c, size_type pos = 0) const noexcept
      {
         return find(basic_string_view(&c, 1), pos);
      }


   private:


      const_pointer m_Ptr;
      size_type m_Size;
   };
}