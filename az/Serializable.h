// Serializable.h
#pragma once
#include "az/Serializer.h"

namespace az {
// Base class with template-based visitor
class Serializable {
   public:
    virtual ~Serializable() = default;

    void serialize(az::TypedSerializer &serializer) const { visitProperties(serializer); }

   protected:
    // Child classes implement this to visit their properties
    virtual void visitProperties(az::TypedSerializer &serializer) const = 0;
};
}  // namespace az