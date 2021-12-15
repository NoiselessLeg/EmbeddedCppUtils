#pragma once
#include "ntl_defines.h"

#ifdef NTL_FEATURE_HAS_OPTIONAL
#include <optional>
#else
#include <atomic>
#include <memory>
#endif

namespace ntl
{
   template <typename T>
#ifdef NTL_FEATURE_HAS_OPTIONAL
   class optional : public std::optional<T>
   {
#else
   class optional
   {
      using value_type = T;

      constexpr optional() noexcept:
         m_HasElem(false)
      {
      }

      constexpr optional(std::nullopt_t) noexcept :
         m_HasElem(false)
      {
      }

      constexpr optional(const optional& other) noexcept :
         m_HasElem(other.m_HasElem)
      {
         memcpy(&m_Elem, &other.m_Elem, sizeof(m_Elem));
      }

      constexpr optional(optional&& other) noexcept :
         m_HasElem(other.m_HasElem)
      {
         memcpy(&m_Elem, &other.m_Elem, sizeof(m_Elem));
      }

      constexpr const T* operator -> () const
      {
         return reinterpret_cast<const T*>(&m_Elem);
      }

      constexpr T* operator -> () 
      {
         return reinterpret_cast<T*>(&m_Elem);
      }

      constexpr const T& operator * () const
      {
         return *(reinterpret_cast<const T*>(&m_Elem));
      }

      constexpr T& operator * ()
      {
         return *(reinterpret_cast<T*>(&m_Elem));
      }

      constexpr const T&& operator * () const
      {
         return *(reinterpret_cast<const T*>(&m_Elem));
      }

      constexpr T& operator * ()
      {
         return *(reinterpret_cast<T*>(&m_Elem));
      }

   private:
      std::atomic<bool> m_HasElem;
      std::aligned_storage_t<sizeof(T), alignof(T)> m_Elem;
#endif
   };
}
