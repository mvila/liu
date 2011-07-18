#ifndef LIU_LANGUAGE_ARGUMENT_H
#define LIU_LANGUAGE_ARGUMENT_H

#include "node/object/pair.h"
#include "node/object/language/primitive.h"

LIU_BEGIN

namespace Language {
    #define LIU_ARGUMENT(ARGS...) \
    new Language::Argument(context()->child("Object", "Language", "Argument"), ##ARGS)

    class Argument : public GenericPair<Primitive *, Primitive *> {
        LIU_DECLARE(Argument, Pair, Language);
    public:
        explicit Argument(Node *origin) :
            GenericPair<Primitive *, Primitive *>(origin, NULL, NULL) {}

        Argument(Node *origin, Primitive *value) :
            GenericPair<Primitive *, Primitive *>(origin, NULL, NULL) { setValue(value); }

        Argument(Node *origin, Primitive *label, Primitive *value) :
            GenericPair<Primitive *, Primitive *>(origin, NULL, NULL) { setLabel(label); setValue(value); }

        Argument(Node *origin, Node *node) :
            GenericPair<Primitive *, Primitive *>(origin, NULL, NULL) { setValue(LIU_PRIMITIVE(node)); }

        virtual ~Argument() {
            setLabel(NULL);
            setValue(NULL);
        }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Argument);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Argument, LIU_FORK_IF_NOT_NULL(label()), LIU_FORK_IF_NOT_NULL(value()));

        virtual void firstValueWillChange() { if(first()) removeAnonymousChild(first()); }
        virtual void firstValueHasChanged() { if(first()) addAnonymousChild(first()); }
        virtual void secondValueWillChange() { if(second()) removeAnonymousChild(second()); }
        virtual void secondValueHasChanged() { if(second()) addAnonymousChild(second()); }

        // aliases...
        Primitive *label() const { return key(); }
        void setLabel(Primitive *label) { setKey(label); }

        QString labelName() const;

        virtual Node *run(Node *receiver = context()) {
            // LIU_PUSH_RUN(this); // is it really necessary?
            return value()->run(receiver);
        }

        virtual QString toString(bool debug = false, short level = 0) const;
    };
}

LIU_END

#endif // LIU_LANGUAGE_ARGUMENT_H
