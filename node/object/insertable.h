#ifndef LIU_INSERTABLE_H
#define LIU_INSERTABLE_H

#include "node/object/indexable.h"

LIU_BEGIN

class Insertable : public Indexable {
    LIU_DECLARE_2(Insertable, Indexable, Object);
public:
    explicit Insertable(Node *origin = context()->child("Object", "Insertable")) : Indexable(origin) {}

    Insertable *init();

    virtual void insert(Node *index, Node *item, Node *before = NULL, bool *wasFoundPtr = NULL) {
        Q_UNUSED(index); Q_UNUSED(item); Q_UNUSED(before); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(insert);
};

LIU_END

#endif // LIU_INSERTABLE_H
