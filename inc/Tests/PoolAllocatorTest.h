#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class PoolAllocatorTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(PoolAllocatorTest)

      protected:
         void execute_impl();
      };
   }
}
