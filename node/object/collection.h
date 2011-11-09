#ifndef LIU_COLLECTION_H
#define LIU_COLLECTION_H

#include "node/object/iterable.h"

LIU_BEGIN

class Collection : public Iterable {
    LIU_DECLARE_2(Collection, Iterable, Object);
public:
    explicit Collection(Node *origin = context()->child("Object", "Collection")) :
        Iterable(origin), _isFlattened(NULL) {}

    Collection *init();

    LIU_DECLARE_ACCESSOR(bool, isFlattened, IsFlattened);
    LIU_DECLARE_PROPERTY(flattened);

    virtual void append(Node *item) { Q_UNUSED(item); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(append);

    virtual Node *remove(Node *item, bool *wasFoundPtr = NULL) { Q_UNUSED(item); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(remove);

    virtual void clear();
    LIU_DECLARE_NATIVE_METHOD(clear);
private:
    bool *_isFlattened;
};

LIU_END

#endif // LIU_COLLECTION_H
