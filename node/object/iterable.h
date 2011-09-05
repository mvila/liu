#ifndef LIU_ITERABLE_H
#define LIU_ITERABLE_H

#include "node/object/message.h"
#include "node/object/boolean.h"
#include "node/object/text.h"

LIU_BEGIN

#define LIU_ITERABLE(ARGS...) new Iterable(context()->child("Object", "Iterable"), ##ARGS)

class Iterable : public Object {
    LIU_DECLARE(Iterable, Object, Object);
public:
    explicit Iterable(Node *origin);

    virtual ~Iterable() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterable);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterable);

    LIU_DECLARE_NATIVE_METHOD(init);

    virtual bool isEqualTo(const Node *other) const;
    LIU_DECLARE_NATIVE_METHOD(equal_to);

    virtual QString toString(bool debug = false, short level = 0) const;
};

LIU_END

#endif // LIU_ITERABLE_H
