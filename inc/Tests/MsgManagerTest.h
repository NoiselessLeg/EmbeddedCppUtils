#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class MsgManagerTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(MsgManagerTest)

      protected:
         void execute_impl();
      };

      struct TestMsg
      {
         uint32_t Id;
         uint32_t Data;
      };
   }
}