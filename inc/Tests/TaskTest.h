#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class TaskTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(TaskTest)

      protected:
         void execute_impl();
      };
   }
}