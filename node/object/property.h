#ifndef LIU_PROPERTY_H
#define LIU_PROPERTY_H

#include "node/object.h"

LIU_BEGIN

#define LIU_PROPERTY(ARGS...) \
new Property(context()->child("Object", "Property"), ##ARGS)

#define LIU_DECLARE_PROPERTY(NAME) \
LIU_DECLARE_NATIVE_METHOD(NAME##_get); \
LIU_DECLARE_NATIVE_METHOD(NAME##_set);

#define LIU_DECLARE_READ_ONLY_PROPERTY(NAME) \
LIU_DECLARE_NATIVE_METHOD(NAME##_get);

#define LIU_DEFINE_NODE_PROPERTY(CLASS, TYPE, NAME, NAME_CAP) \
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(CLASS, NAME); \
LIU_DEFINE_NATIVE_METHOD(CLASS, NAME##_set) { \
    LIU_FIND_LAST_MESSAGE; \
    LIU_CHECK_INPUT_SIZE(1); \
    TYPE *value = TYPE::dynamicCast(message->runFirstInput()); \
    if(!value) LIU_THROW_TYPECAST_EXCEPTION(QString("%1 is expected").arg(#TYPE)); \
    CLASS::cast(parent())->set##NAME_CAP(value); \
    return this; \
}

#define LIU_DEFINE_READ_ONLY_NODE_PROPERTY(CLASS, NAME) \
LIU_DEFINE_NATIVE_METHOD(CLASS, NAME##_get) { \
    LIU_FIND_LAST_MESSAGE; \
    LIU_CHECK_INPUT_SIZE(0); \
    if(message->isQuestioned()) \
        LIU_TODO; \
    else \
        return CLASS::cast(parent())->NAME(); \
}

#define LIU_ADD_PROPERTY(CLASS, NAME) \
Property *NAME##Property = LIU_PROPERTY(); \
NAME##Property->LIU_ADD_NATIVE_METHOD(CLASS, NAME##_get, get); \
NAME##Property->LIU_ADD_NATIVE_METHOD(CLASS, NAME##_set, set); \
addChild(#NAME, NAME##Property);

#define LIU_ADD_READ_ONLY_PROPERTY(CLASS, NAME) \
Property *NAME##Property = LIU_PROPERTY(); \
NAME##Property->LIU_ADD_NATIVE_METHOD(CLASS, NAME##_get, get); \
addChild(#NAME, NAME##Property);

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
