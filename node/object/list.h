#ifndef LIU_LIST_H
#define LIU_LIST_H

#include "node/object/insertable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/property.h"
#include "node/object/language/message.h"

LIU_BEGIN

class List : public Insertable {
    LIU_DECLARE_2(List, Object, Object);
public:
    friend class Dictionary;

    explicit List(Node *origin = context()->child("Object", "List")) :
        Insertable(origin), _operations(NULL) {}

    List *init();

    LIU_DECLARE_NATIVE_METHOD(make);

    virtual void inspectInternal() const;

    // --- Iterable ---

    class Iterator;
    virtual Iterator *iterator() const;
    virtual int size() const;

    // --- Collection ---

    virtual void append(Node *item);
    virtual Node *remove(Node *item, bool *wasFoundPtr = NULL);
    virtual void clear();

    // --- Indexable ---

    virtual Node *get(Node *index, bool *wasFoundPtr = NULL);
private:
    Node *_get(int index);
public:
    virtual void set(Node *index, Node *item, bool *wasFoundPtr = NULL);
private:
    void _set(int index, Node *item);
public:
    virtual void append(Node *index, Node *item, bool *okPtr = NULL);
    virtual Node *unset(Node *index, bool *wasFoundPtr = NULL);
private:
    void _unset(int index);
public:
    virtual bool indexIsSequential() const { return true; }

    // --- Insertable ---

    virtual void insert(Node *index, Node *item, Node *before = NULL, bool *okPtr = NULL);
private:
    void _insert(int index, Node *item);
public:
private:
    int _partition(int left, int right, int pivotIndex);
    void _quickSort(int left, int right);
public:
    void sort() { _quickSort(0, size() - 1); }
    LIU_DECLARE_NATIVE_METHOD(sort);
private:
    struct Operation {
        enum Type { Null, Set, Insert, Remove };
        Operation(const Type theType, const int theIndex, const int theSize, const QList<Node *> theData = QList<Node *>()) :
            type(theType), index(theIndex), size(theSize), data(theData) {};
        Type type;
        int index;
        int size;
        QList<Node *> data;
    };
    QList<Operation> *_operations;
public:
    // === Iterator ===

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE_2(Iterator, Iterable::Iterator, List);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "List", "Iterator")) :
            Iterable::Iterator(origin), _source(NULL), _index(NULL) {};

        static Iterator *make(List *source) { return (new Iterator())->init(source); }

        Iterator *init(List *source = NULL, int *index = NULL);

        LIU_DECLARE_NODE_ACCESSOR(List, source, Source);
        LIU_DECLARE_READ_ONLY_PROPERTY(source);

        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual NodeQPair peekNext() const;
        virtual void skipNext();
    private:
        List *_source;
        int *_index;
    };
};

LIU_END

#endif // LIU_LIST_H
