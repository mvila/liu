#ifndef LIU_LIST_H
#define LIU_LIST_H

#include "node/object/insertable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"

LIU_BEGIN

class List : public Insertable {
    LIU_DECLARE(List, Object, Object);
public:
    explicit List(Node *origin = context()->child("Object", "List")) :
        Insertable(origin), _operations(NULL) {}

    static List *make() { return (new List())->init(); }

    List *init();
    List *initCopy(const List *other);

    virtual ~List();

    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(List);
    LIU_DECLARE_AND_DEFINE_COPY_METHOD_2(List);

    LIU_DECLARE_NATIVE_METHOD(make);

    virtual QString toString(bool debug = false, short level = 0) const;
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
    class IndexIterator;
    virtual IndexIterator *indexIterator() const;

    // --- Insertable ---

    virtual void insert(Node *index, Node *item, bool *okPtr = NULL);
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
        LIU_DECLARE(Iterator, Object, List);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "List", "Iterator")) :
            Iterable::Iterator(origin), _list(NULL), _index(NULL) {};

        static Iterator *make() { return (new Iterator())->init(); }
        static Iterator *make(List *list) { return (new Iterator())->init(&list); }

        Iterator *init(List **list = NULL, int *index = NULL);
        Iterator *initCopy(const Iterator *other);

        virtual ~Iterator();

        LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Iterator);
        LIU_DECLARE_AND_DEFINE_COPY_METHOD_2(Iterator);

        LIU_DECLARE_ACCESSOR(ListPtr, list, List);
        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual Node *peekNext() const;
        virtual void skipNext();
    private:
        List **_list;
        int *_index;
    };

    // === IndexIterator ===

    class IndexIterator : public Iterable::Iterator {
        LIU_DECLARE(IndexIterator, Object, List);
    public:
        explicit IndexIterator(Node *origin = context()->child("Object", "List", "IndexIterator")) :
            Iterable::Iterator(origin), _list(NULL), _index(NULL) {};

        static IndexIterator *make() { return (new IndexIterator())->init(); }
        static IndexIterator *make(List *list) { return (new IndexIterator())->init(&list); }

        IndexIterator *init(List **list = NULL, int *index = NULL);
        IndexIterator *initCopy(const IndexIterator *other);

        virtual ~IndexIterator();

        LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(IndexIterator);
        LIU_DECLARE_AND_DEFINE_COPY_METHOD_2(IndexIterator);

        LIU_DECLARE_ACCESSOR(ListPtr, list, List);
        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual Number *peekNext() const;
        virtual void skipNext();
    private:
        List **_list;
        int *_index;
    };
};

LIU_END

#endif // LIU_LIST_H
