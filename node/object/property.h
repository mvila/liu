#ifndef LIU_PROPERTY_H
#define LIU_PROPERTY_H

#include "node/object.h"

LIU_BEGIN

#define LIU_PROPERTY(ARGS...) \
new Property(context()->child("Object", "Property"), ##ARGS)

class Property : public Object {
    LIU_DECLARE(Property, Object, Object);
public:
    explicit Property(Node *origin) : Object(origin) { setIsAutoRunnable(true); }

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Property);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Property);

    virtual Node *run(Node *receiver = context()) {
        Q_UNUSED(receiver);
        LIU_PUSH_RUN(this);
        return child("get")->run(this);
    }
};

LIU_END

#endif // LIU_PROPERTY_H
