#ifndef LIU_LANGUAGE_PRIMITIVE_H
#define LIU_LANGUAGE_PRIMITIVE_H

#include "node/object/property.h"
#include "node/object/language.h"

LIU_BEGIN

namespace Language {
    #define LIU_PRIMITIVE_ADD(BASE, OTHER) \
    if(BASE) BASE->last()->setNext(OTHER); else BASE = (OTHER);

    class Primitive : public Object {
        LIU_DECLARE_2(Primitive, Object, Language);
    public:
        explicit Primitive(Node *origin = context()->child("Object", "Language", "Primitive")) :
            Object(origin), _value(NULL), _sourceCodeRef(NULL), _next(NULL), _previous(NULL) {}

        static Primitive *make(Node *value) { return (new Primitive())->init(value); }

        static Primitive *make(Node *value, const QStringRef &sourceCodeRef) {
            return (new Primitive())->init(value, &sourceCodeRef);
        }

        Primitive *init(Node *value = NULL, const QStringRef *sourceCodeRef = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Node, value, Value);
        LIU_DECLARE_PROPERTY(value);

        LIU_DECLARE_ACCESSOR(QStringRef, sourceCodeRef, SourceCodeRef);

        LIU_DECLARE_NODE_ACCESSOR(Primitive, next, Next);
        LIU_DECLARE_PROPERTY(next);
        Primitive *last();
        int size() const;

        Primitive *previous() const { return _previous; }
        bool hasPrevious() const { return _previous; }

        virtual Node *run(Node *receiver = context());

        virtual Node *unnamedChild(int index) const;

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        Node *_value;
        QStringRef *_sourceCodeRef;
        Primitive *_next;
        Primitive *_previous;
    public:
        // === Skip ===

        class Skip {
        public:
            Skip(Node *result = NULL) : result(result) {}

            Node *result;
        };

        static void skip(Node *result) __attribute__ ((noreturn)) { throw Skip(result); }
    };

    #define LIU_FIND_LAST_PRIMITIVE Primitive *primitive = findLastPrimitive();

    inline Primitive *findLastPrimitive(RunStack *stack = runStack(), bool *okPtr = NULL) {
        return stack->find<Primitive>(okPtr);
    }
}

LIU_END

#endif // LIU_LANGUAGE_PRIMITIVE_H
