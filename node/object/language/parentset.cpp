#include "node/object/language/parentset.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(ParentSet, Object, Language);

    ParentSet *ParentSet::init(Node *source) {
        Iterable::init();
        setSource(source);
        return this;
    }

    ParentSet *ParentSet::initCopy(const ParentSet *other) {
        Iterable::initCopy(other);
        setSource(other->_source);
        return this;
    }

    ParentSet::~ParentSet() {
        setSource();
    }

    void ParentSet::initRoot() {
        addExtension(Iterable::root());

        setSource(Node::root()->fork());

        LIU_ADD_NATIVE_METHOD(ParentSet, init);

        LIU_ADD_READ_ONLY_PROPERTY(ParentSet, source)
    }

    LIU_DEFINE_NATIVE_METHOD(ParentSet, init) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        setSource(message->runFirstInput());
        return this;
    }

    LIU_DEFINE_NODE_ACCESSOR(ParentSet, Node, source, Source);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(ParentSet, source);
    LIU_DEFINE_READ_ONLY_NODE_PROPERTY(ParentSet, source);

    // --- Iterable ---

    ParentSet::Iterator *ParentSet::iterator() const {
        return ParentSet::Iterator::make(source());
    }

    // === NamedChildDictionary::Iterator ===

    LIU_DEFINE_2(ParentSet::Iterator, Iterable::Iterator, ParentSet);

    ParentSet::Iterator *ParentSet::Iterator::init(Node *source) {
        Iterable::Iterator::init();
        setSource(source);
        return this;
    }

    ParentSet::Iterator *ParentSet::Iterator::initCopy(const ParentSet::Iterator *other) {
        Iterable::Iterator::initCopy(other);
        setSource(other->_source);
        return this;
    }

    ParentSet::Iterator::~Iterator() {
        setSource();
        unsetSourceIterator();
    }

    void ParentSet::Iterator::initRoot() {
        setSource(Node::root()->fork());

        LIU_ADD_READ_ONLY_PROPERTY(ParentSet::Iterator, source)
    }

    LIU_DEFINE_NODE_ACCESSOR(ParentSet::Iterator, Node, source, Source);

    void ParentSet::Iterator::sourceWillChange() {}
    void ParentSet::Iterator::sourceHasChanged() { unsetSourceIterator(); }

    LIU_DEFINE_READ_ONLY_NODE_PROPERTY(ParentSet::Iterator, source);

    ParentSet::Iterator::SourceIterator *ParentSet::Iterator::sourceIterator() const {
        if(!_sourceIterator) {
            if(!source()) LIU_THROW_NULL_POINTER_EXCEPTION("source is NULL");
            if(source()->_parents) constCast(this)->_sourceIterator = new SourceIterator(*source()->_parents);
        }
        return _sourceIterator;
    }

    void ParentSet::Iterator::unsetSourceIterator() {
        if(_sourceIterator) {
            delete _sourceIterator;
            _sourceIterator = NULL;
        }
    }

    bool ParentSet::Iterator::hasNext() const {
        return sourceIterator() ? sourceIterator()->hasNext() : false;
    }

    NodeQPair ParentSet::Iterator::peekNext() const {
        if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
        return NodeQPair(NULL, sourceIterator()->peekNext().key());
    }

    void ParentSet::Iterator::skipNext() {
        if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
        sourceIterator()->next();
    }
}

LIU_END
