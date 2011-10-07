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
    explicit List(Node *origin = context()->child("Object", "List")) : Insertable(origin) {}

    //    List(const List &other);

    static List *make() { return (new List())->init(); }

    List *init();

    virtual ~List();

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(List);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(List);

    virtual QString toString(bool debug = false, short level = 0) const;

    // --- Iterable ---

    class Iterator;
    virtual Iterator *iterator() const;
    virtual int size() const;

    // --- Collection ---

    virtual void append(Node *item);
    virtual Node *remove(Node *item, bool *wasFoundPtr = NULL);

    // --- Indexable ---

    virtual Node *get(Node *nodeIndex, bool *wasFoundPtr = NULL);
private:
    Node *_get(int index);
public:
    virtual void set(Node *nodeIndex, Node *nodeValue, bool *wasFoundPtr = NULL);
    virtual void append(Node *nodeIndex, Node *item, bool *okPtr = NULL);
    virtual Node *unset(Node *nodeIndex, bool *wasFoundPtr = NULL);
    class IndexIterator;
    virtual IndexIterator *indexIterator() const;

    // --- Insertable ---

    virtual void insert(Node *nodeIndex, Node *nodeValue, bool *okPtr = NULL);
private:
    void _insert(int index, Node *item);
public:
private:
    struct Operation {
        enum Type { Null, Set, Insert, Remove };
        Operation(const Type theType = Null, const int theIndex = 0,
                  const int theSize = 0, QList<Node *> *theData = NULL) :
            type(theType), index(theIndex), size(theSize), data(theData) {};
        Type type;
        int index;
        int size;
        QList<Node *> *data;
    };

    QList<Operation> *_operations;
public:
    // === Iterator ===

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE(Iterator, Object, List);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "List", "Iterator")) :
            Iterable::Iterator(origin) {};

        static Iterator *make() { return (new Iterator())->init(); }
        static Iterator *make(List *list) { return (new Iterator())->init(&list); }

        Iterator *init(List **list = NULL, int *index = NULL);

        virtual ~Iterator();

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterator);

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
            Iterable::Iterator(origin) {};

        static IndexIterator *make() { return (new IndexIterator())->init(); }
        static IndexIterator *make(List *list) { return (new IndexIterator())->init(&list); }

        IndexIterator *init(List **list = NULL, int *index = NULL);

        virtual ~IndexIterator();

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(IndexIterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(IndexIterator);

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
