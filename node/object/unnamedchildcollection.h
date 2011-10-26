#ifndef LIU_UNNAMED_CHILD_COLLECTION_H
#define LIU_UNNAMED_CHILD_COLLECTION_H

#include "node/object/iterable.h"

LIU_BEGIN

class UnnamedChildCollection : public Iterable {
    LIU_DECLARE_2(UnnamedChildCollection, Object, Object);
public:
    explicit UnnamedChildCollection(Node *origin = context()->child("Object", "UnnamedChildCollection")) :
        Iterable(origin), _source(NULL) {}

    static UnnamedChildCollection *make(Node *source) { return (new UnnamedChildCollection())->init(source); }

    UnnamedChildCollection *init(Node *source = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_NODE_ACCESSOR(Node, source, Source);
    LIU_DECLARE_READ_ONLY_PROPERTY(source);

    // --- Iterable ---

    class Iterator;
    virtual Iterator *iterator() const;
private:
    Node *_source;
public:
     // === Iterator ===

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE_2(Iterator, Iterable::Iterator, UnnamedChildCollection);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "UnnamedChildCollection", "Iterator")) :
            Iterable::Iterator(origin), _source(NULL), _index(NULL) {};

        static Iterator *make(const Node *source) { return (new Iterator())->init(Node::constCast(source)); }

        Iterator *init(Node *source = NULL, int *index = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Node, source, Source);
        LIU_DECLARE_READ_ONLY_PROPERTY(source);

        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual NodeQPair peekNext() const;
        virtual void skipNext();
    private:
        Node *_source;
        int *_index;
    };
};

LIU_END

#endif // LIU_UNNAMED_CHILD_COLLECTION_H
