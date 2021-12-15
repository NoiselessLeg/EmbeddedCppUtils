#pragma once
#include "ntl_defines.h"
#include "bounded_queue.h"
#include "bounded_vector.h"
#include "thread_util.h"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <tuple>

namespace ntl
{
   static constexpr std::size_t MAX_NUM_CONCURRENT_TASKS = MAX_NUM_NTL_TASKS;

   namespace detail
   {
      enum class TaskState : uint8_t
      {
         TASK_PENDING_ACTIVATION,
         TASK_WAITING,
         TASK_PREPARING_TO_EXECUTE,
         TASK_EXECUTING
      };

      struct WaitBlock
      {
         WaitBlock() = default;
         WaitBlock(WaitBlock&& rhs) = default;
         WaitBlock& operator = (WaitBlock && rhs) = default;
         std::mutex WaitMtx;
         std::condition_variable WaitCond;
      };

      template <typename ... Args>
      struct TaskParams
      {
         using func_type = std::function<void(Args...)>;

         TaskParams()
         {
         }

         template <typename Func>
         TaskParams(Func f, Args&& ... args) :
            Method(f),
            Parameters(std::make_tuple(std::forward<Args>(args)...))
         {
         }

         TaskParams(TaskParams&& rhs) = default;

         TaskParams& operator = (TaskParams && rhs) = default;

         func_type Method;
         std::tuple<Args...> Parameters;
      };

      template <>
      struct TaskParams<void>
      {
         using func_type = std::function<void()>;

         TaskParams()
         {
         }

         template <typename Func>
         TaskParams(Func f) :
            Method(f)
         {
         }

         TaskParams(const TaskParams& rhs) = default;

         TaskParams& operator = (const TaskParams & rhs) = default;

         TaskParams(TaskParams&& rhs) = default;

         TaskParams& operator = (TaskParams && rhs) = default;

         func_type Method;
      };

      template <typename ... Args>
      class task
      {
      public:
         using queue_type = ntl::bounded_queue<TaskParams<Args...>, MAX_NUM_CONCURRENT_TASKS>;

         task(uint32_t id) noexcept :
            m_IsRunning(true),
            m_State(TaskState::TASK_PENDING_ACTIVATION),
            m_Id(id),
            m_pWaitBlock(std::make_unique<WaitBlock>()),
            m_Underlying([&]() -> void { thread_main();  })
         {
         }

         task(const task& rhs) noexcept = delete;

         task& operator = (const task& rhs) = delete;

         task(task&& rhs) noexcept = delete;

         task& operator = (task&& rhs) = delete;

         ~task()
         {
            if (m_IsRunning)
            {
               terminate();
               m_Underlying.join();
            }
         }


         template <typename FuncType>
         void enqueue_task(FuncType func, Args&&... args)
         {
            std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
            detail::TaskParams<Args...> item(func, std::forward<Args...>(args)...);
            m_TaskQ.enqueue(item);
            m_pWaitBlock->WaitCond.notify_one();
         }

         typename queue_type::size_type get_num_enqueued_tasks() const
         {
            std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
            return queue_size_impl();
         }

         detail::TaskState get_task_state() const
         {
            return m_State;
         }

         void terminate()
         {
            std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
            m_IsRunning = false;
            m_pWaitBlock->WaitCond.notify_one();
         }

      private:

#ifdef HAS_CPP17
         template <typename Callable, typename Tuple>
         auto call(Callable callable, Tuple t)
         {
            return std::apply(callable, t);
         }
#else
         template <typename Callable, typename Tuple, size_t ... I>
         constexpr auto call(Callable callable, Tuple t, std::index_sequence<I...>)
         {
            return callable(std::get<I>(t)...);
         }

         template <typename Callable, typename Tuple>
         constexpr auto call(Callable callable, Tuple t)
         {
            static constexpr auto size = std::tuple_size<Tuple>::value;
            return call(callable, t, std::make_index_sequence<size>());
         }

#endif

         typename queue_type::size_type queue_size_impl() const
         {
            return m_TaskQ.size();
         }

         void thread_main()
         {
            while (m_IsRunning)
            {
               m_State = TaskState::TASK_WAITING;
               NTL_DEBUG_PRINTF("Thread {}: entering wait block\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
               std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
               m_pWaitBlock->WaitCond.wait(lockGuard, [&]() -> bool {
                  bool shouldStopWaiting = !m_IsRunning || m_TaskQ.size() > 0;
                  return shouldStopWaiting;
                  });

               m_State = TaskState::TASK_PENDING_ACTIVATION;
               if (get_num_enqueued_tasks() > 0)
               {
                  detail::TaskParams<void> taskData = std::move(m_TaskQ.front());
                  m_TaskQ.pop();
                  m_State = detail::TaskState::TASK_EXECUTING;
                  lockGuard.unlock();
                  NTL_DEBUG_PRINTF("Thread %u: got task and doing things\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
                  call(taskData.Method);
               }
               else
               {
                  NTL_DEBUG_PRINTF("Thread %u: did not dequeue a task\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
                  m_State = TaskState::TASK_WAITING;
               }

            }

            NTL_DEBUG_PRINTF("Thread %u: exiting thread_main()\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
         }

         std::atomic<bool> m_IsRunning;
         std::atomic<detail::TaskState> m_State;
         uint32_t m_Id;
         std::unique_ptr<WaitBlock> m_pWaitBlock;
         std::thread m_Underlying;
         queue_type m_TaskQ;
      };

      template <>
      class task<void>
      {
      public:
         using queue_type = ntl::bounded_queue<TaskParams<void>, MAX_NUM_CONCURRENT_TASKS>;

         task(uint32_t id) noexcept:
            m_IsRunning(true),
            m_State(TaskState::TASK_PENDING_ACTIVATION),
            m_Id(id),
            m_pWaitBlock(std::make_unique<WaitBlock>()),
            m_Underlying([&]() { thread_main();  })
         {
         }

         task(const task& rhs) noexcept = delete;

         task& operator = (const task& rhs) = delete;

         task(task&& rhs) noexcept = delete;

         task& operator = (task&& rhs) = delete;

         ~task()
         {
            if (m_IsRunning)
            {
               terminate();
               if (m_Underlying.joinable())
               {
                  m_Underlying.join();
               }
            }
         }

         template <typename FuncType>
         void enqueue_task(FuncType func)
         {
            NTL_DEBUG_PRINTF("Emplacing function on task {}\n", m_Id);
            detail::TaskParams<void> taskParms(func);

            std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
            m_TaskQ.push(std::move(taskParms));
            m_pWaitBlock->WaitCond.notify_one();
         }

         typename queue_type::size_type get_num_enqueued_tasks() const
         {
            std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
            return queue_size_impl();
         }

         detail::TaskState get_task_state() const
         {
            return m_State;
         }

         void terminate()
         {
            std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
            m_IsRunning = false;
            m_pWaitBlock->WaitCond.notify_one();
         }

      private:

         template <typename Callable>
         auto call(Callable callable)
         {
            callable();
         }

         typename queue_type::size_type queue_size_impl() const
         {
            return m_TaskQ.size();
         }

         void thread_main()
         {
            while (m_IsRunning)
            {
               m_State = TaskState::TASK_WAITING;
               NTL_DEBUG_PRINTF("Thread {}: entering wait block\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
               std::unique_lock<decltype(m_pWaitBlock->WaitMtx)> lockGuard(m_pWaitBlock->WaitMtx);
               m_pWaitBlock->WaitCond.wait(lockGuard, [&]() {
                  bool shouldStopWaiting = !m_IsRunning || m_TaskQ.size() > 0;
                  return shouldStopWaiting;
               });

               m_State = TaskState::TASK_PENDING_ACTIVATION;
               if (queue_size_impl() > 0)
               {
                  detail::TaskParams<void> taskData = std::move(m_TaskQ.front());
                  m_TaskQ.pop();
                  m_State = detail::TaskState::TASK_EXECUTING;
                  NTL_DEBUG_PRINTF("Thread {}: got task and doing things\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
                  call(taskData.Method);
               }
               else
               {
                  NTL_DEBUG_PRINTF("Thread {}: did not dequeue a task\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
                  m_State = TaskState::TASK_WAITING;
               }

            }

            NTL_DEBUG_PRINTF("Thread {}: exiting thread_main()\n", thread_util::get_unique_thread_id(std::this_thread::get_id()));
         }

         std::atomic<bool> m_IsRunning;
         std::atomic<detail::TaskState> m_State;
         uint32_t m_Id;
         std::unique_ptr<WaitBlock> m_pWaitBlock;
         std::thread m_Underlying;
         queue_type m_TaskQ;
      };
   }

   template <typename ...Args>
   class TaskPool
   {
   public:
      TaskPool()
      {
         for (size_t i = 0; i < m_TaskVec.capacity(); ++i)
         {
            m_TaskVec.emplace_back(i);
         }
      }

      template <typename FuncType>
      void enqueue_task(FuncType func, Args&&... args)
      {
         std::unique_lock<decltype(m_VecMtx)> lockGuard(m_VecMtx);
         auto candidateTaskItr = find_candidate_task();
         assert(candidateTaskItr != m_TaskVec.end());
         candidateTaskItr->enqueue_task(func, std::forward<Args>(args)...);
      }


   private:
      using task_container = ntl::bounded_vector<detail::task<void>, MAX_NUM_NTL_TASK_HANDLERS>;

      typename task_container::iterator find_candidate_task()
      {
         auto least_busy_task = std::min_element(m_TaskVec.begin(), m_TaskVec.end(), [](const detail::task<Args...>& lhs, const detail::task<Args...>& rhs) {
            return lhs.get_num_enqueued_tasks() < rhs.get_num_enqueued_tasks();
         });

         return least_busy_task;
      }

      std::mutex m_VecMtx;
      task_container m_TaskVec;
   };

   template <>
   class TaskPool<void>
   {
   public:
      TaskPool()
      {
         for (size_t i = 0; i < m_TaskVec.capacity(); ++i)
         {
            m_TaskVec.emplace_back(i);
         }
      }

      template <typename FuncType>
      void enqueue_task(FuncType func)
      {
         std::unique_lock<decltype(m_VecMtx)> lockGuard(m_VecMtx);
         auto candidateTaskItr = find_candidate_task();
         assert(candidateTaskItr != m_TaskVec.end());
         candidateTaskItr->enqueue_task(func);
      }


   private:
      using task_container = ntl::bounded_vector<detail::task<void>, MAX_NUM_NTL_TASK_HANDLERS>;

      typename task_container::const_iterator find_candidate_task() const
      {
         auto least_busy_task = std::min_element(m_TaskVec.begin(), m_TaskVec.end(), [](const detail::task<void>& lhs, const detail::task<void>& rhs) {
            return lhs.get_num_enqueued_tasks() < rhs.get_num_enqueued_tasks();
            });

         return least_busy_task;
      }

      typename task_container::iterator find_candidate_task()
      {
         auto least_busy_task = std::min_element(m_TaskVec.begin(), m_TaskVec.end(), [](const detail::task<void>& lhs, const detail::task<void>& rhs) {
            return lhs.get_num_enqueued_tasks() < rhs.get_num_enqueued_tasks();
         });

         return least_busy_task;
      }

      std::mutex m_VecMtx;
      task_container m_TaskVec;
   };


}
