#pragma once
#include <chrono>
#include <mutex>

namespace ntl
{
   class shared_mutex
   {
   public:
      shared_mutex();

      shared_mutex(const shared_mutex& rhs) = delete;
      shared_mutex& operator = (const shared_mutex& rhs) = delete;

      void lock();
      bool try_lock();

      template <class Rep, class Period>
      bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time)
      {
         return try_lock_until(std::chrono::steady_clock::now() + rel_time);
      }

      template <class Clock, class Duration>
      bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time)
      {
         std::unique_lock<mutex_t> lock(m_Mtx);
         if (m_State & WRITE_ENTERED)
         {
            while (true)
            {
               std::cv_status status = m_Gate1.wait_until(lock, abs_time);
               if ((m_State & WRITE_ENTERED) == 0)
               {
                  break;
               }
               if (status == std::cv_status::timeout)
               {
                  return false;
               }
            }
         }

         m_State |= WRITE_ENTERED;
         if (m_State & NUM_READERS)
         {
            while (true)
            {
               std::cv_status status = m_Gate2.wait_until(lock, abs_time);
               if ((m_State & NUM_READERS) == 0)
               {
                  break;
               }

               if (status == std::cv_status::timeout)
               {
                  m_State &= ~WRITE_ENTERED;
                  return false;
               }
            }
         }

         return true;
      }

      void unlock();

      void lock_shared();
      bool try_lock_shared();

      template <class Rep, class Period>
      bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& rel_time)
      {
         return try_lock_shared_until(std::chrono::steady_clock::now() + rel_time);
      }

      template <class Clock, class Duration>
      bool try_lock_shared_until(const std::chrono::time_point<Clock, Duration>& abs_time)
      {
         std::unique_lock<mutex_t> lock(m_Mtx);
         if ((m_State & WRITE_ENTERED) || (m_State & NUM_READERS) == NUM_READERS)
         {
            std::cv_status status = m_Gate1.wait_until(lock, abs_time);
            if ((m_State & WRITE_ENTERED) == 0 &&
                (m_State & NUM_READERS) < NUM_READERS)
            {
               break;
            }

            if (status == std::cv_status::timeout)
            {
               return false;
            }
         }

         count_t numReaders = (m_State & NUM_READERS) + 1;
         m_State &= ~NUM_READERS;
         m_State |= numReaders;
         return true;
      }

      void unlock_shared();

   private:
      using mutex_t  = std::mutex;
      using cond_t   = std::condition_variable;
      using count_t  = unsigned;

      mutex_t  m_Mtx;
      cond_t   m_Gate1;
      cond_t   m_Gate2;
      count_t  m_State;

      static constexpr count_t WRITE_ENTERED = 1U << (sizeof(count_t) * CHAR_BIT - 1);
      static constexpr count_t NUM_READERS   = ~WRITE_ENTERED;
   };

   template <class Mutex>
   class shared_lock
   {
   public:
      using mutex_type = Mutex;

      explicit shared_lock(mutex_type& m) :
         m_Mtx(m),
         m_IsOwner(true)
      {
         m.lock_shared();
      }

      shared_lock(mutex_type& m, std::defer_lock_t) :
         m_Mtx(m),
         m_IsOwner(false)
      {
      }

      shared_lock(mutex_type& m, std::try_to_lock_t) :
         m_Mtx(m),
         m_IsOwner(m.try_lock_shared())
      {
      }

      template <class Clock, class Duration>
      shared_lock(mutex_type& m, const std::chrono::time_point<Clock, Duration>& abs_time):
         m_Mtx(m),
         m_IsOwner(m.try_lock_shared_until(abs_time))
      {
      }

      template <class Rep, class Period>
      shared_lock(mutex_type& m, const std::chrono::duration<Rep, Period>& rel_time):
         m_Mtx(m),
         m_IsOwner(m.try_lock_shared_for(rel_time))
      {
      }

      ~shared_lock()
      {
         if (m_IsOwner)
         {
            m_Mtx.unlock_shared();
         }
      }

      void swap(shared_lock&& u)
      {
         std::swap(m_Mtx, u.m_Mtx);
         std::swap(m_IsOwner, u.m_IsOwner);
      }

   private:
      mutex_type& m_Mtx;
      bool m_IsOwner;
   };
}

namespace std
{
   template <class Mutex>
   void swap(ntl::shared_lock<Mutex>& x, ntl::shared_lock<Mutex>& y) noexcept
   {
      x.swap(y);
   }
}