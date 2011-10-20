#ifndef LIU_NAMED_CHILD_DICTIONARY_H
#define LIU_NAMED_CHILD_DICTIONARY_H

#include "node/object/indexable.h"

LIU_BEGIN

class NamedChildDictionary : public Indexable {
    LIU_DECLARE_2(NamedChildDictionary, Indexable, Object);
public:
    explicit NamedChildDictionary(Node *origin = context()->child("Object", "NamedChildDictionary")) :
        Indexable(origin), _source(NULL) {}

    static NamedChildDictionary *make(Node *source) { return (new NamedChildDictionary())->init(source); }

    NamedChildDictionary *init(Node *source = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_NODE_ACCESSOR(Node, source, Source);
    LIU_DECLARE_READ_ONLY_PROPERTY(source);

    // --- Iterable ---

    class Iterator;
    virtual Iterator *iterator() const;

    // --- Indexable ---

    virtual Node *get(Node *index, bool *wasFoundPtr = NULL);
private:
    Node *_source;
public:
    // === Iterator ===

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE_2(Iterator, Object, NamedChildDictionary);
    public:
        typedef QHashIterator<QString, Node *> SourceIterator;

        explicit Iterator(Node *origin = context()->child("Object", "NamedChildDictionary", "Iterator")) :
            Iterable::Iterator(origin), _source(NULL), _sourceIterator(NULL) {};

        static Iterator *make(NamedChildDictionary *source) { return (new Iterator())->init(source); }

        Iterator *init(NamedChildDictionary *theSource = NULL);

        LIU_DECLARE_NODE_ACCESSOR(NamedChildDictionary, source, Source);
        LIU_DECLARE_READ_ONLY_PROPERTY(source);

        SourceIterator *sourceIterator() const;
        void unsetSourceIterator();

        virtual bool hasNext() const;
        virtual Node *peekNext() const;
        virtual void skipNext();
    private:
        NamedChildDictionary *_source;
        SourceIterator *_sourceIterator;
    };
};

LIU_END

#endif // LIU_NAMED_CHILD_DICTIONARY_H
