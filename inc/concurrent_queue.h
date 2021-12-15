#pragma once
#include <atomic>
#include <memory>


namespace ntl
{

   namespace detail
   {
      template <typename T>
      class ccq_node
      {


      private:
         std::aligned_storage_t<sizeof(T), alignof(T)> m_Elem;
      };
   }

   enum class queue_op_status
   {
      success = 0,
      empty,
      full,
      closed
   };

   template <typename T, size_t MaxSize>
   class concurrent_queue
   {
   public:
      typedef T value_type;

      static bool is_lock_free() noexcept;

      void push(const T& elem)
      {
         size_t 
      }

      void push(T&& elem);

      T value_pop();

      queue_op_status try_push(const T& elem);

      queue_op_status try_push(T&& elem);

      void close() noexcept;
      bool is_closed() const noexcept;

      queue_op_status wait_push(const T& elem);
      queue_op_status wait_push(T&& elem);

      queue_op_status wait_pop(T& elem);

      void open();

      bool is_empty() const noexcept;

      bool is_full() const noexcept;

   private:
      T* to_pointer(std::aligned_storage_t<sizeof(T)>, alignof(T)>& elem) noexcept
      {
         return reinterpret_cast<T*>(&elem);
      }

      const T* to_pointer(const std::aligned_storage_t<sizeof(T)>, alignof(T)>& elem) const noexcept
      {
         return reinterpret_cast<const T*>(&elem);
      }

      std::array<detail::ccq_node<T>, MaxSize> m_Array;

      std::atomic<size_t> m_TailIdx;
      std::atomic<size_t> m_HeadIdx;

      std::atomic<bool> m_IsOpen;

   };
}