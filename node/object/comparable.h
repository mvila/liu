#ifndef LIU_COMPARABLE_H
#define LIU_COMPARABLE_H

#include "node/object.h"

LIU_BEGIN

class Comparable : public Object {
    LIU_DECLARE_2(Comparable, Object, Object);
public:
    explicit Comparable(Node *origin = context()->child("Object", "Comparable")) : Object(origin) {}

    Comparable *init();

    LIU_DECLARE_NATIVE_METHOD(compare);
    LIU_DECLARE_NATIVE_METHOD(less_than);
    LIU_DECLARE_NATIVE_METHOD(less_than_or_equal_to);
    LIU_DECLARE_NATIVE_METHOD(greater_than);
    LIU_DECLARE_NATIVE_METHOD(greater_than_or_equal_to);
};

LIU_END

#endif // LIU_COMPARABLE_H
