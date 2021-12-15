#pragma once
#include "read_write_lock.h"
#include "TestBase.h"

namespace ntl
{
   namespace testing
   {
      class ReadWriteLockTest : public TestBase
      {
      public:
         NTL_DEFINE_DERIVED_TEST_CONSTRUCTOR(ReadWriteLockTest)

      protected:
         void execute_impl();

      private:
         template <typename IdType>
         void print_read_message(const IdType& id);

         template <typename IdType>
         void print_read_done_message(const IdType& id);

         template <typename IdType>
         void print_write_message(const IdType& id);

         template <typename IdType>
         void print_write_done_message(const IdType& id);

         void read_impl();
         void write_impl();

         ntl::shared_mutex m_RwLock;

         std::mutex m_PrintMtx;
      };
   }
}