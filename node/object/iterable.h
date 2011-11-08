#ifndef LIU_ITERABLE_H
#define LIU_ITERABLE_H

#include "node/object/boolean.h"
#include "node/object/language/message.h"

LIU_BEGIN

// === Iterable ===

class Iterable : public Object {
    LIU_DECLARE_2(Iterable, Object, Object);
public:
    explicit Iterable(Node *origin = context()->child("Object", "Iterable")) : Object(origin) {}

    Iterable *init();

    virtual QString toString(bool debug = false, short level = 0) const;

    class Iterator;
    virtual Iterator *iterator() const { LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(iterator);

    virtual Node *getValue(Node *value, bool *wasFoundPtr = NULL);
    LIU_DECLARE_NATIVE_METHOD(get);

    virtual bool contains(Node *value) const;
    LIU_DECLARE_NATIVE_METHOD(contains);

    virtual int count(Node *value) const;
    LIU_DECLARE_NATIVE_METHOD(count);

    virtual int size() const;
    LIU_DECLARE_NATIVE_METHOD(size);

    virtual bool empty() const;
    LIU_DECLARE_NATIVE_METHOD(empty);

    virtual NodeQPair first(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(first);

    virtual NodeQPair second(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(second);

    virtual NodeQPair third(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(third);

    virtual NodeQPair last(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(last);

    virtual const QString join(const QString &separator = "", const QString &prefix = "",
                       const QString &suffix = "", bool debug = false, short level = 0) const;

    // === Iterator ===

    class Iterator : public Object {
        LIU_DECLARE_2(Iterator, Object, Iterable);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "Iterable", "Iterator")) : Object(origin) {}

        Iterator *init();

        virtual bool hasNext() const { LIU_ABSTRACT_CALL; }
        virtual NodeQPair peekNext() const { LIU_ABSTRACT_CALL; }
        virtual void skipNext() { LIU_ABSTRACT_CALL; }
        NodeQPair next(bool *wasFoundPtr = NULL);

        LIU_DECLARE_NATIVE_METHOD(read);
        LIU_DECLARE_NATIVE_METHOD(skip);
    };
};

LIU_END

#endif // LIU_ITERABLE_H
