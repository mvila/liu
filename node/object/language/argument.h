#ifndef LIU_LANGUAGE_ARGUMENT_H
#define LIU_LANGUAGE_ARGUMENT_H

#include "node/object/pair.h"
#include "node/object/language/primitive.h"

LIU_BEGIN

namespace Language {
    class Argument : public Pair {
        LIU_DECLARE_2(Argument, Pair, Language);
    public:
        explicit Argument(Node *origin = context()->child("Object", "Language", "Argument")) :
            Pair(origin) {}

        static Argument *make(Primitive *value) { return (new Argument())->init(NULL, value); }
        static Argument *make(Primitive *label, Primitive *value) { return (new Argument())->init(label, value); }
        static Argument *make(Node *node) { return (new Argument())->init(NULL, Primitive::make(node)); }

        Argument *init(Primitive *label = NULL, Primitive *value = NULL);

        Primitive *label() const { return Primitive::cast(first()); }
        Primitive *hasLabel() const { return Primitive::cast(first()); }
        void setLabel(Primitive *label = NULL) { setFirst(label); }
        QString labelName() const;
        LIU_DECLARE_PROPERTY(label);

        Primitive *value() const { return Primitive::cast(second()); }
        Primitive *hasValue() const { return Primitive::cast(second()); }
        void setValue(Primitive *value = NULL) { setSecond(value); }
        LIU_DECLARE_PROPERTY(value);

        virtual Node *run(Node *receiver = context()) {
            // LIU_PUSH_RUN(this); // is it really necessary?
            return value()->run(receiver);
        }

        virtual QString toString(bool debug = false, short level = 0) const;
    };
}

LIU_END

#endif // LIU_LANGUAGE_ARGUMENT_H
