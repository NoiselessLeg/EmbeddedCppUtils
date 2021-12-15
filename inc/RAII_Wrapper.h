#pragma once
#include <functional>

namespace ntl
{
   class RAII_Wrapper
   {
   public:
      template <typename CollectorFunc>
      explicit RAII_Wrapper(CollectorFunc func) :
         m_Destructor(func)
      {
      }

      ~RAII_Wrapper()
      {
         m_Destructor();
      }


   private:
      std::function<void()> m_Destructor;
   };
}
