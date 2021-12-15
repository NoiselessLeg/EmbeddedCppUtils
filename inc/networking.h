#pragma once
#include "ntl_defines.h"

#include <atomic>
#include <stdexcept>

#ifdef NTL_WIND
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#include "Singleton.h"
#else


#endif

namespace ntl
{
   namespace detail
   {
#ifdef NTL_WIND
      class winsock_init
      {
      public:
         winsock_init()
         {
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0)
            {
               NTL_THROW(std::runtime_error, "Failed to initialize WinSock library.");
            }
         }

         winsock_init(const winsock_init& rhs) = delete;
         winsock_init(winsock_init&& rhs) = delete;

         winsock_init& operator =(const winsock_init& rhs) = delete;
         winsock_init& operator =(winsock_init&& rhs) = delete;

         ~winsock_init()
         {
            WSACleanup();
         }
      };
#endif
   }

   enum class connection_type
   {
      tcp,
      udp_unicast,
      udp_multicast
   };

   class socket_base
   {
   public:
      socket_base()
      {
#ifdef NTL_WIND
         // create an encapsulation functor for the winsock init/cleanup routines
         // this will go out of scope on program close, and is guaranteed to only
         // be initialized once.
         ntl::Singleton<detail::winsock_init>::instance();
#endif



      }

   private:

   };
}
