#ifndef LIU_INDEXABLE_H
#define LIU_INDEXABLE_H

#include "node/object/collection.h"

LIU_BEGIN

class Indexable : public Collection {
    LIU_DECLARE_2(Indexable, Collection, Object);
public:
    explicit Indexable(Node *origin = context()->child("Object", "Indexable")) : Collection(origin) {}

    Indexable *init();

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

    virtual bool indexIsSequential() const { LIU_ABSTRACT_CALL; }

    virtual Node *index(Node *item, bool *wasFoundPtr = NULL);
    LIU_DECLARE_NATIVE_METHOD(index);

    virtual const QString join(const QString &separator = "", const QString &prefix = "",
                       const QString &suffix = "", bool debug = false, short level = 0) const;
};

LIU_END

#endif // LIU_INDEXABLE_H
