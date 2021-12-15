#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class BoundedMapTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(BoundedMapTest)

         void execute_impl();
      };
   }
}
