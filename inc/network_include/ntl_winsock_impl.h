#pragma once
#include <WinSock2.h>

namespace ntl
{
   namespace detail
   {
      struct dgram_winsock_data
      {
         SOCKET socket_handle;
      };
   }
}