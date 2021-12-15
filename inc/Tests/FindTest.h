#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class FindTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(FindTest)

      protected:
         void execute_impl();
      };
   }
}