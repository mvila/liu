#ifndef LIU_LANGUAGE_ABSTRACT_METHOD_H
#define LIU_LANGUAGE_ABSTRACT_METHOD_H

#include "node/object.h"
#include "node/object/language/message.h"
#include "node/object/language/parameterlist.h"

LIU_BEGIN

namespace Language {
    #define LIU_ABSTRACT_METHOD(ARGS...) \
    new Language::AbstractMethod(context()->child("Object", "Language", "AbstractMethod"), ##ARGS)

    class AbstractMethod : public Object {
        LIU_DECLARE(AbstractMethod, Object, Language);
    public:
        explicit AbstractMethod(Node *origin, ParameterList *inputs = NULL, ParameterList *outputs = NULL,
                        const QString &codeInputName = "") :
            Object(origin), _inputs(inputs), _outputs(outputs), _codeInputName(codeInputName) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(AbstractMethod);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(AbstractMethod, LIU_FORK_IF_NOT_NULL(inputs(false)),
                                             LIU_FORK_IF_NOT_NULL(outputs(false)), codeInputName());

        ParameterList *inputs(bool createIfNull = true) const {
            if(!_inputs && createIfNull) constCast(this)->_inputs = LIU_PARAMETER_LIST();
            return _inputs;
        }

        void setInputs(ParameterList *inputs) { _inputs = inputs; }

        void setInputs(ArgumentBunch *arguments) {
            if(hasAnInput()) inputs()->clear();
            ArgumentBunch::Iterator i(arguments);
            while(Argument *argument = i.next())
                appendInput(argument);
        }

        Parameter *input(short i) const { return inputs(false)->get(i); }
        Parameter *input(const QString &label) const { return inputs(false)->get(label); }
        bool hasInput(short i) const { return inputs(false) && inputs()->hasIndex(i); }
        bool hasInput(const QString &label) const { return inputs(false) && inputs()->hasLabel(label); }
        bool hasAnInput() const { return hasInput(0); }

        void appendInput(Argument *argument) {
            Primitive *label = argument->label();
            Primitive *defaultValue = argument->value();
            if(!label) {
                label = defaultValue;
                defaultValue = NULL;
            }
            if(label->hasNext())
                LIU_THROW(ArgumentException, "illegal label parameter found in method definition (should be a Message");
            Message *labelMsg = Message::dynamicCast(label->value());
            if(!labelMsg)
                LIU_THROW(ArgumentException, "illegal label parameter found in method definition (should be a Message)");
            inputs()->append(LIU_PARAMETER(labelMsg->name(), defaultValue,
                                             labelMsg->isEscaped(), labelMsg->isParented()));
        }

        ParameterList *outputs(bool createIfNull = true) const {
            if(!_outputs && createIfNull) constCast(this)->_outputs = LIU_PARAMETER_LIST();
            return _outputs;
        }

        void setOutputs(ParameterList *outputs) { _outputs = outputs; }

        const QString &codeInputName() const { return _codeInputName; }
        void setCodeInputName(const QString &name) { _codeInputName = name; }
        bool hasCodeInput() const { return !_codeInputName.isEmpty(); }

        virtual void hasBeenDefined(Message *message) {
            setNodeName(message->name());
            if(message->hasAnInput()) setInputs(message->inputs());
            if(message->hasCodeInput()) setCodeInputName(message->codeInputName());
        }

        QString signature(bool debug = false, short level = 0) const {
            QString str = nodeName();
            str += "(";
            if(inputs(false)) str += inputs()->toString(debug, level);
            str += ")";
            if(hasCodeInput()) str += " " + codeInputName() + "...";
            if(outputs(false) && outputs()->isNotEmpty()) str += " -> " + outputs()->toString(debug, level);
            return str;
        }
    private:
        ParameterList *_inputs;
        ParameterList *_outputs;
        QString _codeInputName;
    };
}

LIU_END

#endif // LIU_LANGUAGE_ABSTRACT_METHOD_H
