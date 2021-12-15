#pragma once
#include <utility>
#include <typeindex>
#include <type_traits>

namespace ntl
{
   template <typename T>
   class observer_ptr
   {
   public:
      using value_type   = T;
      using pointer_type = T*;
      using const_pointer_type = const T*;
      using reference = T&;
      using const_reference = const T&;

      constexpr observer_ptr() noexcept:
         m_Ptr(nullptr)
      {
      }

      constexpr observer_ptr(std::nullptr_t val) noexcept :
         m_Ptr(val)
      {
      }

      constexpr explicit observer_ptr(pointer_type ptr) noexcept :
         m_Ptr(ptr)
      {
      }

      template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
      constexpr observer_ptr(observer_ptr<U> other) noexcept:
         m_Ptr(other.m_Ptr)
      {
      }

      constexpr pointer_type get() const noexcept
      {
         return m_Ptr;
      }

      constexpr const pointer_type get() const noexcept
      {
         return m_Ptr;
      }

      constexpr reference operator *() const
      {
         return *get();
      }

      constexpr const_reference operator *() const
      {
         return *get();
      }

      constexpr pointer_type operator ->() const noexcept
      {
         return get();
      }

      constexpr const_pointer_type operator ->() const noexcept
      {
         return get();
      }

      constexpr explicit operator bool() const noexcept
      {
         return get() != nullptr;
      }

      constexpr explicit operator pointer_type() const noexcept
      {
         return get();
      }

      constexpr explicit operator const_pointer_type() const noexcept
      {
         return get();
      }

      constexpr pointer_type release() noexcept
      {
         pointer_type ret = get();
         m_Ptr = nullptr;
         return ret;
      }

      constexpr void reset(pointer_type ptr = nullptr) noexcept
      {
         m_Ptr = ptr;
      }

      constexpr void swap(observer_ptr& rhs) noexcept
      {
         std::swap(m_Ptr, rhs);
      }

   private:
      pointer_type m_Ptr;
   };

   template <typename T>
   void swap(observer_ptr<T>& p1, observer_ptr<T>& p2) noexcept
   {
      p1.swap(p2);
   }

   template <typename T>
   observer_ptr<T> make_observer(T* p) noexcept
   {
      return observer_ptr<T>{p};
   }

   template <typename T1, typename T2>
   bool operator == (const observer_ptr<T1> p1, const observer_ptr<T2> p2) noexcept
   {
      return p1.get() == p2.get();
   }

   template <typename T1, typename T2>
   bool operator != (const observer_ptr<T1> p1, const observer_ptr<T2> p2) noexcept
   {
      return !(p1 == p2);
   }

   template <typename T>
   bool operator == (const observer_ptr<T> p, std::nullptr_t) noexcept
   {
      return !p;
   }

   template <typename T>
   bool operator == (std::nullptr_t, const observer_ptr<T> p) noexcept
   {
      return !p;
   }

   template <typename T>
   bool operator != (const observer_ptr<T> p, std::nullptr_t) noexcept
   {
      return static_cast<bool>(p);
   }

   template <typename T>
   bool operator != (std::nullptr_t, const observer_ptr<T> p) noexcept
   {
      return static_cast<bool>(p);
   }

   template <typename T1, typename T2>
   bool operator < (const observer_ptr<T1> p1, const observer_ptr<T2> p2)
   {
      return std::less(p1.get(), p2.get());
   }

   template <typename T1, typename T2>
   bool operator > (const observer_ptr<T1> p1, const observer_ptr<T2> p2)
   {
      return p2 < p1;
   }

   template <typename T1, typename T2>
   bool operator <= (const observer_ptr<T1> p1, const observer_ptr<T2> p2)
   {
      return !(p2 < p1);
   }

   template <typename T1, typename T2>
   bool operator >= (const observer_ptr<T1> p1, const observer_ptr<T2> p2)
   {
      return !(p1 < p2);
   }
}
template <typename T>
struct std::hash<ntl::observer_ptr<T>>
{
   std::size_t operator ()(const ntl::observer_ptr<T>& t) const
   {
      return std::hash<T*>(t.get());
   }
};