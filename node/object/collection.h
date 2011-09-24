#ifndef LIU_COLLECTION_H
#define LIU_COLLECTION_H

#include "node/object/iterable.h"

LIU_BEGIN

class Collection : public Iterable {
    LIU_DECLARE(Collection, Iterable, Object);
public:
    explicit Collection(Node *origin = context()->child("Object", "Collection")) : Iterable(origin) {}

    static Collection *make() { return (new Collection())->init(); }

    Collection *init() { Iterable::init(); return this; }

    virtual ~Collection() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Collection);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Collection);

    virtual void append(Node *item) { Q_UNUSED(item); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(append);

    virtual void remove(Node *item, bool *wasFoundPtr = NULL) { Q_UNUSED(item); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(remove);
};

LIU_END

#endif // LIU_COLLECTION_H
