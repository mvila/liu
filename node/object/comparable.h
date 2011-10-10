#ifndef LIU_COMPARABLE_H
#define LIU_COMPARABLE_H

#include "node/object.h"

LIU_BEGIN

class Comparable : public Object {
    LIU_DECLARE(Comparable, Object, Object);
public:
    explicit Comparable(Node *origin = context()->child("Object", "Comparable")) : Object(origin) {}

    static Comparable *make() { return (new Comparable())->init(); }

    Comparable *init() { Object::init(); return this; }

    virtual ~Comparable() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Comparable);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Comparable);

    LIU_DECLARE_NATIVE_METHOD(less_than);
    LIU_DECLARE_NATIVE_METHOD(less_than_or_equal_to);
    LIU_DECLARE_NATIVE_METHOD(greater_than);
    LIU_DECLARE_NATIVE_METHOD(greater_than_or_equal_to);
};

LIU_END

#endif // LIU_COMPARABLE_H
