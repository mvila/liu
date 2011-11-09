#ifndef LIU_LANGUAGE_PARENT_SET_H
#define LIU_LANGUAGE_PARENT_SET_H

#include "node/object/iterable.h"

LIU_BEGIN

namespace Language {
    class ParentSet : public Iterable {
        LIU_DECLARE_2(ParentSet, Object, Language);
    public:
        explicit ParentSet(Node *origin = context()->child("Object", "Language", "ParentSet")) :
            Iterable(origin), _source(NULL) {}

        static ParentSet *make(Node *source) { return (new ParentSet())->init(source); }

        ParentSet *init(Node *source = NULL);

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
            LIU_DECLARE_2(Iterator, Iterable::Iterator, ParentSet);
        public:
            typedef QHashIterator<Node *, HugeUnsignedInteger> SourceIterator;

            explicit Iterator(Node *origin = context()->child("Object", "Language", "ParentSet", "Iterator")) :
                Iterable::Iterator(origin), _source(NULL), _sourceIterator(NULL) {};

            static Iterator *make(const Node *source) { return (new Iterator())->init(Node::constCast(source)); }

            Iterator *init(Node *source = NULL);

            LIU_DECLARE_NODE_ACCESSOR(Node, source, Source);
            LIU_DECLARE_READ_ONLY_PROPERTY(source);

            SourceIterator *sourceIterator() const;
            void unsetSourceIterator();

            virtual bool hasNext() const;
            virtual NodeQPair peekNext() const;
            virtual void skipNext();
        private:
            Node *_source;
            SourceIterator *_sourceIterator;
        };
    };
}

LIU_END

#endif // LIU_LANGUAGE_PARENT_SET_H
