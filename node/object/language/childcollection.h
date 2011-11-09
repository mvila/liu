#ifndef LIU_CHILD_COLLECTION_H
#define LIU_CHILD_COLLECTION_H

#include "node/object/language/namedchilddictionary.h"
#include "node/object/language/unnamedchildcollection.h"

LIU_BEGIN

namespace Language {
    class ChildCollection : public Iterable {
        LIU_DECLARE_2(ChildCollection, Object, Language);
    public:
        explicit ChildCollection(Node *origin = context()->child("Object", "Language", "ChildCollection")) :
            Iterable(origin), _source(NULL) {}

        static ChildCollection *make(Node *source) { return (new ChildCollection())->init(source); }

        ChildCollection *init(Node *source = NULL);

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
            LIU_DECLARE_2(Iterator, Iterable::Iterator, ChildCollection);
        public:
            explicit Iterator(Node *origin = context()->child("Object", "Language", "ChildCollection", "Iterator")) :
                Iterable::Iterator(origin), _source(NULL), _namedChildIterator(NULL), _unnamedChildIterator(NULL),
                _phase(NULL) {};

            static Iterator *make(const Node *source) { return (new Iterator())->init(Node::constCast(source)); }

            Iterator *init(Node *source = NULL);

            LIU_DECLARE_NODE_ACCESSOR(Node, source, Source);
            LIU_DECLARE_READ_ONLY_PROPERTY(source);

            LIU_DECLARE_NODE_ACCESSOR(NamedChildDictionary::Iterator, namedChildIterator, NamedChildIterator);
            LIU_DECLARE_NODE_ACCESSOR(UnnamedChildCollection::Iterator, unnamedChildIterator, UnnamedChildIterator);

            LIU_DECLARE_ACCESSOR(short, phase, Phase);

            Iterable::Iterator *currentIterator() const;

            virtual bool hasNext() const;
            virtual NodeQPair peekNext() const;
            virtual void skipNext();
        private:
            Node *_source;
            NamedChildDictionary::Iterator *_namedChildIterator;
            UnnamedChildCollection::Iterator *_unnamedChildIterator;
            short *_phase;
        };
    };
}

LIU_END

#endif // LIU_CHILD_COLLECTION_H
