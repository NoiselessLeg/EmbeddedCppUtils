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
#include <cassert>
#include <limits>
#include <type_traits>

#ifdef __cpp_exceptions 
#include <stdexcept>
namespace ntl
{
   class overflow_error : public std::runtime_error
   {
   public:
      overflow_error():
         std::runtime_error("Overflow occurred while performing a mathematical operation")
      {
      }
   };

   class divide_by_zero_error : public std::runtime_error
   {
   public:
      divide_by_zero_error():
         std::runtime_error("Divide by zero occurred while performing a mathematical operation")
      {
      }
   };
}

#define NTL_THROW_OR_ABORT(condition, extype) condition ? (void)0 : throw extype()
#else
#include <cstdio>
#define NTL_THROW_OR_ABORT(condition, extype) \
   if (!(condition)) { \
      fprintf(stderr, "Overflow occurred while performing mathematical operation (exceptions disabled - aborting)\n"); \
      std::abort(); \
   } else \
      (void)0
#endif

namespace ntl
{


   template <typename Ty>
   class strong_type
   {
   public:
      explicit constexpr strong_type(const Ty& other) noexcept:
         m_Value(other)
      {
      }

      template <typename U,
                typename = std::enable_if_t<std::is_convertible_v<U, Ty>>>
      explicit constexpr strong_type(const U& other) noexcept:
         m_Value(static_cast<Ty>(other))
      {
      }

      template <typename U,
                typename = std::enable_if_t<std::is_convertible_v<U, Ty>>>
      explicit constexpr strong_type(const strong_type<U>& other) noexcept
      {
         m_Value = static_cast<Ty>(other.m_Value);
      }

      template <typename U,
         typename = std::enable_if_t<std::is_convertible_v<U, Ty>>>
      constexpr strong_type(strong_type<U>&& other) noexcept
      {
         m_Value = std::move(static_cast<Ty>(other.m_Value));
      }

      constexpr strong_type(const strong_type& other) noexcept = default;
      constexpr strong_type& operator =(const strong_type& other) noexcept = default;

      constexpr Ty value() const noexcept
      {
         return m_Value;
      }

      strong_type& operator +=(const strong_type& other)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::max() - other.m_Value >= m_Value, ntl::overflow_error);
         m_Value += other.m_Value;
         return *this;
      }

      strong_type& operator -=(const strong_type& other)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::lowest() + other.m_Value <= m_Value, ntl::overflow_error);
         m_Value -= other.m_Value;
         return *this;
      }

      strong_type& operator *=(const strong_type& other)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::max() / other.m_Value >= m_Value, ntl::overflow_error);
         m_Value *= other.m_Value;
         return *this;
      }

      strong_type& operator /=(const strong_type& other)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::lowest() + other.m_Value <= m_Value, ntl::overflow_error);
         m_Value /= other.m_Value;
         return *this;
      }

      //postfix
      strong_type operator ++(int)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::max() - 1 >= m_Value, ntl::overflow_error);
         strong_type<Ty> ret = *this;
         m_Value++;
         return ret;
      }

      //prefix
      strong_type& operator ++()
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::max() - 1 >= m_Value, ntl::overflow_error);
         ++m_Value;
         return *this;
      }

      //postfix
      strong_type operator --(int)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::lowest() + 1 <= m_Value, ntl::overflow_error);
         strong_type<Ty> ret = *this;
         m_Value--;
         return ret;
      }

      //prefix
      strong_type& operator --()
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::lowest() + 1 <= m_Value, ntl::overflow_error);
         --m_Value;
         return *this;
      }

      friend constexpr strong_type operator + (const strong_type& lhs, const strong_type& rhs)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::max() - rhs.m_Value >= lhs.m_Value, ntl::overflow_error);
         return strong_type<Ty>(rhs.m_Value + lhs.m_Value);
      }

      friend constexpr strong_type operator - (const strong_type& lhs, const strong_type& rhs)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::lowest() + rhs.m_Value <= lhs.m_Value, ntl::overflow_error);
         return strong_type<Ty>(lhs.m_Value - rhs.m_Value);
      }

      friend constexpr strong_type operator * (const strong_type& lhs, const strong_type& rhs)
      {
         NTL_THROW_OR_ABORT(std::numeric_limits<Ty>::max() / rhs.m_Value >= lhs.m_Value, ntl::overflow_error);
         return strong_type<Ty>(rhs.m_Value * lhs.m_Value);
      }

      friend constexpr strong_type operator / (const strong_type& lhs, const strong_type& rhs)
      {
         return strong_type<Ty>(lhs.m_Value / rhs.m_Value);
      }

      friend std::ostream& operator <<(std::ostream& os, const strong_type& val)
      {
         os << val.m_Value;
         return os;
      }

      friend std::istream& operator >>(std::istream& is, strong_type& val)
      {
         is >> val.m_Value;
         return is;
      }


   private:
      template <typename>
      friend class strong_type;

      Ty m_Value;
   };

   template <typename T>
   strong_type<T> make_safetype(const T& value) noexcept
   {
      return strong_type<T>{value};
   }

   using sc_uint8_t  = strong_type<uint8_t>;
   using sc_int8_t   = strong_type<int8_t>;
   using sc_uint16_t = strong_type<uint16_t>;
   using sc_int16_t  = strong_type<int16_t>;
   using sc_int32_t  = strong_type<int32_t>;
   using sc_uint32_t = strong_type<uint32_t>;
}
