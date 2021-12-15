#pragma once
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class CsvReaderTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(CsvReaderTest)

      protected:
         void execute_impl();
      };
   }
}