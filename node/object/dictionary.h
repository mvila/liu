#ifndef LIU_DICTIONARY_H
#define LIU_DICTIONARY_H

#include "node/object/list.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Dictionary : public Insertable {
    LIU_DECLARE_2(Dictionary, Object, Object);
public:
    explicit Dictionary(Node *origin = context()->child("Object", "Dictionary")) :
        Insertable(origin), _items(NULL), _indexes(NULL) {}

    Dictionary *init();

    LIU_DECLARE_NODE_ACCESSOR(List, indexes, Indexes);

    LIU_DECLARE_NATIVE_METHOD(make);

    // --- Iterable ---

    class Iterator;
    virtual Iterator *iterator() const;
    virtual int size() const;

    // --- Collection ---

    virtual void append(Node *item);
    virtual Node *remove(Node *item, bool *wasFoundPtr = NULL);

    // --- Indexable ---

    virtual Node *get(Node *index, bool *wasFoundPtr = NULL);
private:
    Node *_get(Node *index);
public:
    virtual void set(Node *index, Node *item, bool *wasFoundPtr = NULL);
private:
    void _set(Node *index, Node *item);
public:
    virtual void append(Node *index, Node *item, bool *okPtr = NULL);
    virtual Node *unset(Node *index, bool *wasFoundPtr = NULL);
private:
    void _unset(Node *index);
public:
    virtual bool indexIsSequential() const { return false; }

    // --- Insertable ---

//    virtual void insert(Node *index, Node *item, Node *before = NULL, bool *okPtr = NULL);
private:
    void _insert(int position, Node *index, Node *item);
public:
private:
    QHash<Node::Reference, Node *> *_items;
    List *_indexes;
public:
    // === Iterator ===

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE_2(Iterator, Iterable::Iterator, Dictionary);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "Dictionary", "Iterator")) :
            Iterable::Iterator(origin), _source(NULL), _index(NULL) {};

        static Iterator *make(Dictionary *source) { return (new Iterator())->init(source); }

        Iterator *init(Dictionary *source = NULL, int *index = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Dictionary, source, Source);
        LIU_DECLARE_READ_ONLY_PROPERTY(source);

        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual NodeQPair peekNext() const;
        virtual void skipNext();
    private:
        Dictionary *_source;
        int *_index;
    };
};

LIU_END

#endif // LIU_DICTIONARY_H
