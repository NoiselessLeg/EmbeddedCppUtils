#pragma once

namespace ntl
{
   template <typename T>
   class Singleton
   {
   public:
      template <typename ... Args>
      static T& instance(Args&&... args)
      {
         static T instance(std::forward<Args>(args)...);
         return instance;
      }
   };
}
