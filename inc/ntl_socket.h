#pragma once
#include <cstdint>
#include <memory>
#include <string_view>

#ifdef _MSC_VER
#include <WinSock2.h>
#endif

namespace ntl
{
   class dgram_socket
   {
   public:
      dgram_socket(std::string_view ipAddress, uint16_t port);

   private:
      SOCKET m_Handle;
   };

   class stream_socket
   {

   };

   class raw_socket
   {

   };
}