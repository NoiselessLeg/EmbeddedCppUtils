#pragma once
#include "ntl_defines.h"
#include <atomic>
#include <memory>

#ifdef NTL_HAS_EXCEPTIONS
#include <stdexcept>
#endif

namespace ntl
{
#ifdef NTL_HAS_EXCEPTIONS
   class bad_storage_access : public std::exception 
   {

   };
#endif

   template <typename Ty>
   class uninitialized_storage
   {
   public:
      using value_type = Ty;
      using pointer_type = Ty*;
      using reference = Ty&;
      using const_pointer_type = const Ty*;
      using const_reference = const Ty&;

      uninitialized_storage() :
         m_IsInitialized(false)
      {
      }

      template <typename AllocatorTy, typename ... Args>
      uninitialized_storage(AllocatorTy& alloc, Args&& ...args) :
         m_IsInitialized(true)
      {
         construct_impl(alloc, std::forward<Args>(args)...);
      }

      constexpr pointer_type as_pointer() noexcept
      {
         if (!m_IsInitialized)
         {
            NTL_THROW(ntl::bad_storage_access());
         }

         return this->operator->();
      }

      constexpr const_pointer_type as_pointer() const noexcept
      {
         if (!m_IsInitialized)
         {
            NTL_THROW(ntl::bad_storage_access());
         }

         return this->operator->();
      }

      constexpr reference operator *() noexcept
      {
         return *reinterpret_cast<pointer_type>(&m_Strg);
      }

      constexpr const_reference operator *() const noexcept
      {
         return *reinterpret_cast<const_pointer_type>(&m_Strg);
      }

      constexpr pointer_type operator ->() noexcept
      {
         return reinterpret_cast<pointer_type>(&m_Strg);
      }

      constexpr const_pointer_type operator ->() const noexcept
      {
         return reinterpret_cast<const_pointer_type>(&m_Strg);
      }

      template <typename AllocatorTy, typename ... Args>
      bool construct(AllocatorTy& alloc, Args&& ... args) noexcept
      {
         bool success = true;
         try
         {
            construct_impl(alloc, std::forward<Args>(args)...);
            m_IsInitialized = true;
         }
         catch (...)
         {
            success = false;
         }

         return success;
      }

      template <typename AllocatorTy>
      bool destroy(AllocatorTy& alloc) noexcept
      {
         bool success = true;
         try
         {
            if (m_IsInitialized)
            {
               std::allocator_traits<decltype(alloc)>::destroy(alloc, reinterpret_cast<pointer_type>(&m_Strg));
               m_IsInitialized = false;
            }
         }
         catch (...)
         {
            success = false;
         }

         return success;
      }

   private:
      template <typename AllocatorTy, typename ... Args>
      void construct_impl(AllocatorTy& alloc, Args&& ... args)
      {
         std::allocator_traits<decltype(alloc)>::construct(alloc, reinterpret_cast<pointer_type>(&m_Strg), 
            std::forward<Args>(args)...);
      }

      std::atomic<bool> m_IsInitialized;
      std::aligned_storage_t<sizeof(value_type), alignof(value_type)> m_Strg;
   };
}
