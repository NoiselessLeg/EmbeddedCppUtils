#pragma once
#include "ntl_defines.h"

#include <chrono>
#include <cstddef>
#include <functional>

#ifdef NTL_WIND_32
#include <WinUser.h>
#endif

namespace ntl
{
   enum class TimerSyncType
   {
      Sync = 0,
      Async = 7
   };

   template <typename IdType = std::size_t>
   class Timer
   {
   public:
      using timer_id_type = IdType;
      
      template <typename CallbackFunc, class Rep, class Period>
      Timer(timer_id_type id,
            CallbackFunc timeout_func
#ifdef NTL_WIND_32
            ):
         m_ProcId(GetCurrentProcess())
      {
         // this is a microsoft recommendation 
         // - see (https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setuserobjectinformationw)
         BOOL result = SetUserObjectInformation(m_ProcId, UOI_TIMERPROC_EXCEPTION_SUPPRESSION, FALSE, sizeof(FALSE));
         if (FALSE == result)
         {
            NTL_THROW(std::system_error, std::error_code);
         }
         m_WndPrcId = SetTimer(m_ProcId, 0, std::chrono::duration_cast<std::chrono::milliseconds>(timeout), m_Callback);
#else
         {

#endif
         
      }

      constexpr timer_id_type get_id() const noexcept
      {
         return m_Id;
      }

   private:
#ifdef NTL_WIND_32
      HANDLE m_ProcId;
      HANDLE m_WndPrcId;
#endif
      timer_id_type m_Id;
      std::function<void(timer_id_type)> m_Callback;
   };
}