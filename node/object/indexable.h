#ifndef LIU_INDEXABLE_H
#define LIU_INDEXABLE_H

#include "node/object/collection.h"

LIU_BEGIN

class Indexable : public Collection {
    LIU_DECLARE(Indexable, Collection, Object);
public:
    explicit Indexable(Node *origin = context()->child("Object", "Indexable")) : Collection(origin) {}

    static Indexable *make() { return (new Indexable())->init(); }

    Indexable *init() { Collection::init(); return this; }

    virtual ~Indexable() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Indexable);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Indexable);

    virtual Node *get(Node *index, bool *wasFoundPtr = NULL) {
        Q_UNUSED(index); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(get);

    virtual void set(Node *index, Node *item, bool *wasFoundPtr = NULL) {
        Q_UNUSED(index); Q_UNUSED(item); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(set);

    virtual void append(Node *index, Node *item, bool *okPtr = NULL) {
        Q_UNUSED(index); Q_UNUSED(item); Q_UNUSED(okPtr); LIU_ABSTRACT_CALL; }

    void appendOrSet(Node *index, Node *item, bool *okPtr = NULL);
    LIU_DECLARE_NATIVE_METHOD(append_or_set);

    virtual Node *unset(Node *index, bool *wasFoundPtr = NULL) {
        Q_UNUSED(index); Q_UNUSED(wasFoundPtr); LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(unset);
};

LIU_END

#endif // LIU_INDEXABLE_H
