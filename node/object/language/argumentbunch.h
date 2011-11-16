#ifndef LIU_LANGUAGE_ARGUMENT_BUNCH_H
#define LIU_LANGUAGE_ARGUMENT_BUNCH_H

#include "node/object/bunch.h"
#include "node/object/language/argument.h"

LIU_BEGIN

namespace Language {
    #define LIU_ARGUMENT_BUNCH(ARGS...) \
    new Language::ArgumentBunch(context()->child("Object", "Language", "ArgumentBunch"), ##ARGS)

    #define LIU_CHECK_INPUT_SIZE(ARGS...) \
    ArgumentBunch::checkSpecifiedSize(message->inputs(false) ? message->inputs()->size() : 0, ##ARGS)

    #define LIU_CHECK_QUESTION_MARK \
    if(!message->isQuestioned()) LIU_THROW(InterpreterException, "missing question mark");

    #define LIU_CHECK_EXCLAMATION_MARK \
    if(!message->isExclaimed()) LIU_THROW(InterpreterException, "missing exclamation mark");

    class ArgumentBunch : public GenericList<Argument *> {
        LIU_DECLARE(ArgumentBunch, Bunch, Language);
    public:
        explicit ArgumentBunch(Node *origin) : GenericList<Argument *>(origin, true) {}

        ArgumentBunch(Node *origin, Argument *argument) :
            GenericList<Argument *>(origin, argument, true) {}

        ArgumentBunch(Node *origin, Argument *argument1, Argument *argument2) :
            GenericList<Argument *>(origin, argument1, argument2, true) {}

        ArgumentBunch(Node *origin, Node *argument) :
            GenericList<Argument *>(origin, Argument::make(argument), true) {}

        ArgumentBunch(Node *origin, Node *argument1, Node *argument2) :
            GenericList<Argument *>(
                origin, Argument::make(argument1), Argument::make(argument2), true) {}

        ArgumentBunch(Node *origin, OldList *list) : GenericList<Argument *>(origin, true) {
            if(list) {
                OldList::Iterator i(list);
                while(Node *node = i.next()) {
                    Primitive *primitive = Primitive::dynamicCast(node);
                    if(!primitive) LIU_THROW(ArgumentException, "Primitive expected");
                    append(primitive); // FIXME: multiple hasChanged() triggered
                }
            }
        }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(ArgumentBunch);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(ArgumentBunch);

        using GenericList<Argument *>::append;
        void append(Primitive *value);
        void append(Primitive *label, Primitive *value);
        void append(const Bunch *bunch);

        void checkSize(short min, short max = -1) { checkSpecifiedSize(size(), min, max); }
        static void checkSpecifiedSize(short size, short min, short max = -1);

        virtual QString toString(bool debug = false, short level = 0) const {
            return join(", ", "", "", debug, level);
        }
    };
}

LIU_END

#endif // LIU_LANGUAGE_ARGUMENT_BUNCH_H
