#pragma once


namespace ntl
{
   namespace testing
   {
      class ITest
      {
      public:
         virtual ~ITest() {}
         virtual void executeTest() = 0;
         virtual const char* getTestName() const = 0;
      };
   }
}