#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class PunCastTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(PunCastTest)

      protected:
         void execute_impl();
      };
   }
}