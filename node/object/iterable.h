#ifndef LIU_ITERABLE_H
#define LIU_ITERABLE_H

#include "node/object/boolean.h"
#include "node/object/language/message.h"

LIU_BEGIN

// === Iterable ===

class Iterable : public Object {
    LIU_DECLARE(Iterable, Object, Object);
public:
    explicit Iterable(Node *origin = context()->child("Object", "Iterable")) : Object(origin) {}

    static Iterable *make() { return (new Iterable())->init(); }

    Iterable *init() { Object::init(); return this; }

    virtual ~Iterable() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterable);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Iterable);

    class Iterator;
    virtual Iterator *iterator() const { LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(iterator);

    virtual bool contains(Node *value) const;
    LIU_DECLARE_NATIVE_METHOD(contains);

    virtual int count(Node *value) const;
    LIU_DECLARE_NATIVE_METHOD(count);

    virtual int size() const;
    LIU_DECLARE_NATIVE_METHOD(size);

    virtual bool empty() const;
    LIU_DECLARE_NATIVE_METHOD(empty);

    virtual Node *first(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(first);

    virtual Node *second(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(second);

    virtual Node *third(bool *wasFoundPtr = NULL) const;
    LIU_DECLARE_NATIVE_METHOD(third);
public:
    // === Iterator ===

    class Iterator : public Object {
        LIU_DECLARE(Iterator, Object, Iterable);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "Iterable", "Iterator")) : Object(origin) {}

        static Iterator *make() { return (new Iterator())->init(); }

        Iterator *init() { Object::init(); return this; }

        virtual ~Iterator() {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Iterator);

        virtual bool hasNext() const { LIU_ABSTRACT_CALL; }
        virtual Node *peekNext() const { LIU_ABSTRACT_CALL; }
        virtual void skipNext() { LIU_ABSTRACT_CALL; }
        Node *next(bool *wasFoundPtr = NULL);

        LIU_DECLARE_NATIVE_METHOD(read);
        LIU_DECLARE_NATIVE_METHOD(skip);
    };
};

LIU_END

#endif // LIU_ITERABLE_H
