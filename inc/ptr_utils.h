#pragma once
#include <memory>

namespace ntl
{
   namespace memory
   {

      template <typename T, typename AllocTy>
      class allocator_deleter
      {
      public:
         allocator_deleter(const AllocTy& alloc) noexcept:
            m_Alloc(alloc)
         {
         }

         void operator ()(T* ptr)
         {
            if (ptr)
            {
               std::allocator_traits<AllocTy>::destroy(m_Alloc, ptr);
               std::allocator_traits<AllocTy>::deallocate(m_Alloc, ptr, 1);
            }
         }

      private:
         AllocTy m_Alloc;
      };

      template <typename T, typename AllocTy = std::allocator<T>, typename ... Args>
      std::unique_ptr<T, allocator_deleter<T, AllocTy>> allocate_unique(const AllocTy& allocator, Args&&... args)
      {
         auto delFunc = allocator_deleter<T, AllocTy>(allocator);
         T* ptr = nullptr;

         try
         {
            ptr = std::allocator_traits<decltype(allocator)>::allocate(allocator, 1);
            std::allocator_traits<decltype(allocator)>::construct(allocator, ptr, std::forward<Args>(args)...);
         }
         catch (...)
         {
            delFunc(ptr);
            throw;
         }

         return std::unique_ptr<T, allocator_deleter<T, AllocTy>>(ptr, delFunc);
      }
   }
}