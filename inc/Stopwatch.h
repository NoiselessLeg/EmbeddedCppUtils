#pragma once
#include <atomic>
#include <chrono>

#define ENABLE_FCN_PROFILING

namespace ntl
{
   class Stopwatch
   {
   public:
      Stopwatch();

      Stopwatch(const Stopwatch& rhs) = default;
      Stopwatch(Stopwatch&& rhs) = default;

      Stopwatch& operator =(const Stopwatch & rhs) = default;
      Stopwatch& operator =(Stopwatch && rhs) = default;

      std::chrono::duration<double> elapsed() const;

      void reset();
      void start();
      void stop();

   private:
      std::atomic<bool> m_IsRunning;
      std::chrono::high_resolution_clock::time_point m_StartTime;
      std::chrono::high_resolution_clock::time_point m_EndTime;
   };
}

#ifdef ENABLE_FCN_PROFILING
#define START_METHOD_PROFILE() \
   ntl::Stopwatch ZZZ_METHOD_SW();\
   ZZZ_METHOD_SW.start();


#define END_METHOD_PROFILE() \
   ZZZ_METHOD_SW.stop();
#else
#define START_METHOD_PROFILE() static_cast<void>(0);
#define STOP_METHOD_PROFILE() static_cast<void>(0);
#endif
