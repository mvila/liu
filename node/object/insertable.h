#ifndef LIU_INSERTABLE_H
#define LIU_INSERTABLE_H

#include "node/object/indexable.h"

LIU_BEGIN

class Insertable : public Indexable {
    LIU_DECLARE(Insertable, Indexable, Object);
public:
    explicit Insertable(Node *origin = context()->child("Object", "Insertable")) : Indexable(origin) {}

    static Insertable *make() { return (new Insertable())->init(); }

    Insertable *init() { Indexable::init(); return this; }

    virtual ~Insertable() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Insertable);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Insertable);

    virtual void insert(Node *index, Node *item, bool *wasFoundPtr = NULL) {
        Q_UNUSED(index); Q_UNUSED(item); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(insert);
};

LIU_END

#endif // LIU_INSERTABLE_H
