#include "node/object/language/primitive.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Primitive, Object, Language);

    Primitive *Primitive::init(Node *value, const QStringRef *sourceCodeRef) {
        Object::init();
        setValue(value);
        setSourceCodeRef(sourceCodeRef);
        // if(hasValue()) _value->setIsVirtual(false);
        return this;
    }

    Primitive *Primitive::initCopy(const Primitive *other) {
        Object::initCopy(other);
        setValue(other->_value);
        setSourceCodeRef(other->_sourceCodeRef);
        if(other->_next) setNext(other->_next->copy());
        return this;
    }

    Primitive::~Primitive() {
        setValue();
        setSourceCodeRef();
        setNext();
    }

    void Primitive::initRoot() {
//        setValue(Node::root()->fork());
//        setNext(Primitive::root()->fork());

        LIU_ADD_PROPERTY(Primitive, value);
        LIU_ADD_PROPERTY(Primitive, next);
    }

    LIU_DEFINE_NODE_ACCESSOR(Primitive, Node, value, Value);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Primitive, value);
    LIU_DEFINE_NODE_PROPERTY(Primitive, Node, value, Value);

    LIU_DEFINE_ACCESSOR(Primitive, QStringRef, sourceCodeRef, SourceCodeRef,);

    LIU_DEFINE_NODE_ACCESSOR(Primitive, Primitive, next, Next);
    void Primitive::nextWillChange() { if(Primitive *nextPrimitive = hasNext()) nextPrimitive->_previous = NULL; }
    void Primitive::nextHasChanged() { if(Primitive *nextPrimitive = hasNext()) nextPrimitive->_previous = this; }
    LIU_DEFINE_NODE_PROPERTY(Primitive, Primitive, next, Next);

    Primitive *Primitive::last() {
        Primitive *primitive = this;
        while(Primitive *nextPrimitive = primitive->hasNext()) primitive = nextPrimitive;
        return primitive;
    }

    int Primitive::size() const {
        int result = 1;
        const Primitive *primitive = this;
        while((primitive = primitive->hasNext())) result++;
        return result;
    }

    Node *Primitive::run(Node *receiver) {
        Node *result;
        try {
            LIU_PUSH_RUN(this);
            result = value()->run(receiver);
        } catch(const Primitive::Skip &skip) {
            return skip.result;
        }
        Primitive *nextPrimitive = hasNext();
        return nextPrimitive ? result->receive(nextPrimitive) : result;
    }

    QString Primitive::toString(bool debug, short level) const {
        QString str;
        const Primitive *primitive = this;
        bool first = true;
        do {
            if(!first) str += " "; else first = false;
            str += primitive->hasValue() ? primitive->value()->toString(debug, level) :
                                           primitive->Object::toString(debug, level);
            primitive = primitive->hasNext();
        } while(primitive);
        return str;
    }
}

LIU_END
