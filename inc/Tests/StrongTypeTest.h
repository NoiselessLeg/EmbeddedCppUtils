#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class StrongTypeTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(StrongTypeTest)

      protected:
         void execute_impl();
      };
   }
}

