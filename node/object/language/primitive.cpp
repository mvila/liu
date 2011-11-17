#include "node/object/language/primitive.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Primitive, Object, Language);

    Primitive *Primitive::init(Node *value, const QStringRef *sourceCodeRef) {
        Object::init();
        setValue(value);
        setSourceCodeRef(sourceCodeRef);
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
        setValue(Node::root());
        setNext(Primitive::root());

        LIU_ADD_PROPERTY(Primitive, value);
        LIU_ADD_PROPERTY(Primitive, next);
    }

    LIU_DEFINE_NODE_ACCESSOR(Primitive, Node, value, Value);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Primitive, value);
    LIU_DEFINE_NODE_PROPERTY(Primitive, Node, value, Value);

    LIU_DEFINE_ACCESSOR(Primitive, QStringRef, sourceCodeRef, SourceCodeRef,);

    LIU_DEFINE_NODE_ACCESSOR(Primitive, Primitive, next, Next);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Primitive, next);
    LIU_DEFINE_NODE_PROPERTY(Primitive, Primitive, next, Next);

    Primitive *Primitive::last() {
        Primitive *last = this;
        while(Primitive *next = last->hasNext()) last = next;
        return last;
    }

    Primitive *Primitive::beforeLast() {
        Primitive *before = NULL;
        Primitive *last = this;
        while(Primitive *next = last->hasNext()) {
            before = last;
            last = next;
        }
        return before;
    }

    int Primitive::size() const {
        int result = 1;
        const Primitive *primitive = this;
        while((primitive = primitive->hasNext())) result++;
        return result;
    }

    Node *Primitive::run(Node *receiver) {
        Node *result = NULL;
        try {
            LIU_PUSH_RUN(this);
            result = value()->run(receiver);
        } catch(const Primitive::Skip &skip) {
            return skip.result;
        }
        if(Primitive *nextPrimitive = hasNext())
            result = result->receive(nextPrimitive);
        return result;
    }

    Node *Primitive::unnamedChild(int index) const {
        if(_value && _value->isReal()) {
            if(index == 0) return _value;
            index--;
        }
        if(_next && _next->isReal()) {
            if(index == 0) return _next;
            index--;
        }
        return NULL;
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
