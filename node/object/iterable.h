#ifndef LIU_ITERABLE_H
#define LIU_ITERABLE_H

#include "node/object/message.h"
#include "node/object/boolean.h"

LIU_BEGIN

// === Iterable ===

#define LIU_ITERABLE(ARGS...) new Iterable(context()->child("Object", "Iterable"), ##ARGS)

class Iterable : public Object {
    LIU_DECLARE(Iterable, Object, Object);
public:
    explicit Iterable(Node *origin);

    virtual ~Iterable() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterable);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterable);

    LIU_DECLARE_NATIVE_METHOD(init);

    class Iterator;
    virtual Iterator *iterator() const { LIU_ABSTRACT_CALL; }
    LIU_DECLARE_NATIVE_METHOD(iterator);

    virtual bool contains(Node *what) const;
    LIU_DECLARE_NATIVE_METHOD(contains);

    virtual int count(Node *what) const;
    LIU_DECLARE_NATIVE_METHOD(count);

    virtual int size() const;
    LIU_DECLARE_NATIVE_METHOD(size);

    virtual bool empty() const;
    LIU_DECLARE_NATIVE_METHOD(empty);

    virtual bool isEqualTo(const Node *other) const;
    LIU_DECLARE_NATIVE_METHOD(equal_to);

//    virtual QString toString(bool debug = false, short level = 0) const;
public:
    // === Iterator ===

    #define LIU_ITERATOR(ARGS...) new Iterator(context()->child("Object", "Iterable", "Iterator"), ##ARGS)

    class Iterator : public Object {
        LIU_DECLARE(Iterator, Object, Iterable);
    public:
        explicit Iterator(Node *origin);

        virtual ~Iterator() {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterator);

        LIU_DECLARE_NATIVE_METHOD(init);

        virtual bool hasNext() const { LIU_ABSTRACT_CALL; }
        virtual Node *peekNext() const { LIU_ABSTRACT_CALL; }
        virtual void skipNext() { LIU_ABSTRACT_CALL; }
        Node *next();

        LIU_DECLARE_NATIVE_METHOD(read);
        LIU_DECLARE_NATIVE_METHOD(skip);

        virtual bool isEqualTo(const Node *other) const;
        LIU_DECLARE_NATIVE_METHOD(equal_to);

    //    virtual QString toString(bool debug = false, short level = 0) const;
    };
};

LIU_END

#endif // LIU_ITERABLE_H
