#include "node/object/language/namedchilddictionary.h"
#include "node/object/text.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(NamedChildDictionary, Object, Language);

    NamedChildDictionary *NamedChildDictionary::init(Node *source) {
        Indexable::init();
        setSource(source);
        return this;
    }

    NamedChildDictionary *NamedChildDictionary::initCopy(const NamedChildDictionary *other) {
        Indexable::initCopy(other);
        setSource(other->_source);
        return this;
    }

    NamedChildDictionary::~NamedChildDictionary() {
        setSource();
    }

    void NamedChildDictionary::initRoot() {
        addExtension(Indexable::root());

        setSource(Node::root());

        LIU_ADD_NATIVE_METHOD(NamedChildDictionary, init);

        LIU_ADD_READ_ONLY_PROPERTY(NamedChildDictionary, source)
    }

    LIU_DEFINE_NATIVE_METHOD(NamedChildDictionary, init) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        setSource(message->runFirstInput());
        return this;
    }

    LIU_DEFINE_NODE_ACCESSOR(NamedChildDictionary, Node, source, Source);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(NamedChildDictionary, source);
    LIU_DEFINE_READ_ONLY_NODE_PROPERTY(NamedChildDictionary, source);

    // --- Iterable ---

    NamedChildDictionary::Iterator *NamedChildDictionary::iterator() const {
        return NamedChildDictionary::Iterator::make(source());
    }

    // --- Indexable ---

    Node *NamedChildDictionary::get(Node *index, bool *wasFoundPtr) {
        QString name = index->toString();
        Node *result = hasDirectChild(name);
        if(result && result->isVirtual()) result = NULL;
        bool wasFound = result;
        if(wasFoundPtr)
            *wasFoundPtr = wasFound;
        else if(!wasFound)
            LIU_THROW(IndexOutOfBoundsException, "child not found");
        return result;
    }

    // === NamedChildDictionary::Iterator ===

    LIU_DEFINE_2(NamedChildDictionary::Iterator, Iterable::Iterator, NamedChildDictionary);

    NamedChildDictionary::Iterator *NamedChildDictionary::Iterator::init(Node *source) {
        Iterable::Iterator::init();
        setSource(source);
        return this;
    }

    NamedChildDictionary::Iterator *NamedChildDictionary::Iterator::initCopy(const NamedChildDictionary::Iterator *other) {
        Iterable::Iterator::initCopy(other);
        setSource(other->_source);
        return this;
    }

    NamedChildDictionary::Iterator::~Iterator() {
        setSource();
        unsetSourceIterator();
    }

    void NamedChildDictionary::Iterator::initRoot() {
        setSource(Node::root());

        LIU_ADD_READ_ONLY_PROPERTY(NamedChildDictionary::Iterator, source)
    }

    LIU_DEFINE_NODE_ACCESSOR(NamedChildDictionary::Iterator, Node, source, Source);

    void NamedChildDictionary::Iterator::sourceWillChange() {}
    void NamedChildDictionary::Iterator::sourceHasChanged() { unsetSourceIterator(); }

    LIU_DEFINE_READ_ONLY_NODE_PROPERTY(NamedChildDictionary::Iterator, source);

    NamedChildDictionary::Iterator::SourceIterator *NamedChildDictionary::Iterator::sourceIterator() const {
        if(!_sourceIterator) {
            if(!source()) LIU_THROW_NULL_POINTER_EXCEPTION("source is NULL");
            if(source()->_children) constCast(this)->_sourceIterator = new SourceIterator(*source()->_children);
        }
        return _sourceIterator;
    }

    void NamedChildDictionary::Iterator::unsetSourceIterator() {
        if(_sourceIterator) {
            delete _sourceIterator;
            _sourceIterator = NULL;
        }
    }

    bool NamedChildDictionary::Iterator::hasNext() const {
        if(!sourceIterator()) return false;
        while(true) {
            if(!sourceIterator()->hasNext()) return false;
            Node *node = sourceIterator()->peekNext().value();
            if(!node || node->isVirtual()) {
                sourceIterator()->next();
                continue;
            }
            const QString &name = sourceIterator()->peekNext().key();
            if(name.startsWith(".")) {
                sourceIterator()->next();
                continue;
            }
            return true;
        }
    }

    NodeQPair NamedChildDictionary::Iterator::peekNext() const {
        if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
        return NodeQPair(Text::make(sourceIterator()->peekNext().key()), sourceIterator()->peekNext().value());
    }

    void NamedChildDictionary::Iterator::skipNext() {
        if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
        sourceIterator()->next();
    }
}

LIU_END
