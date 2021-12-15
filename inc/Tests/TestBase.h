#pragma once
#include <string>
#include "ITest.h"

namespace ntl
{
   namespace testing
   {
      class TestBase : public ITest
      {
      public:
         TestBase(const std::string& testName);

         void executeTest();

         const char* getTestName() const
         {
            return m_TestName.c_str();
         }

      protected:
         virtual void execute_impl() = 0;

      private:
         std::string m_TestName;
      };
   }
}

#define NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(TypeName) TypeName():TestBase(#TypeName){}