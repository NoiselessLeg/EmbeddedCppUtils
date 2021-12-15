#pragma once
#include <stdexcept>

namespace ntl
{
   class out_of_space : public std::runtime_error
   {
   public:
      out_of_space() :
         std::runtime_error("Out of space to perform the requested operation")
      {
      }

      out_of_space(const char* what) :
         std::runtime_error(what)
      {
      }


   };
}