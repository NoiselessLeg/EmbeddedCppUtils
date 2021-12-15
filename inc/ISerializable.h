#pragma once

namespace ntl
{
   template <typename StreamTy>
   class ISerializable
   {
   public:
      virtual ~ISerializable() {}


      virtual void serialize(StreamTy& strm) const = 0;

      virtual void deserialize(StreamTy& strm) = 0;

   };
}
