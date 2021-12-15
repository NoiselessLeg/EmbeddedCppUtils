#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class BoundedVectorTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(BoundedVectorTest)

      protected:
         void execute_impl();
      };
   }
}