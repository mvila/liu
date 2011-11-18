#ifndef LIU_LANGUAGE_ABSTRACT_METHOD_H
#define LIU_LANGUAGE_ABSTRACT_METHOD_H

#include "node/object.h"
#include "node/object/language/message.h"
#include "node/object/language/parameterlist.h"

LIU_BEGIN

namespace Language {
    class AbstractMethod : public Object {
        LIU_DECLARE_2(AbstractMethod, Object, Language);
    public:
        explicit AbstractMethod(Node *origin = context()->child("Object", "Language", "AbstractMethod")) :
            Object(origin), _inputs(NULL), _outputs(NULL), _codeInputName(NULL) {}

        AbstractMethod *init(ParameterList *inputs = NULL, ParameterList *outputs = NULL, const QString *codeInputName = NULL);

        LIU_DECLARE_NODE_ACCESSOR(ParameterList, inputs, Inputs);

        void setInputs(ArgumentBunch *arguments) {
            inputs()->clear();
            ArgumentBunch::Iterator i(arguments);
            while(Argument *argument = i.next())
                appendInput(argument);
        }

        Parameter *input(short i) const { return inputs()->get(i); }
        Parameter *input(const QString &label) const { return inputs()->get(label); }
        bool hasInput(short i) const { return inputs()->hasIndex(i); }
        bool hasInput(const QString &label) const { return inputs()->hasLabel(label); }
        bool hasAnInput() const { return hasInput(0); }

        void appendInput(Argument *argument);

        LIU_DECLARE_NODE_ACCESSOR(ParameterList, outputs, Outputs);

        LIU_DECLARE_ACCESSOR(QString, codeInputName, CodeInputName);

        void runParameters();

        virtual void hasBeenDefined(Message *message) {
            setNodeName(message->name());
            if(message->hasAnInput()) setInputs(message->inputs());
            if(message->hasCodeInput()) setCodeInputName(message->codeInputName());
        }

        QString signature(bool debug = false, short level = 0) const {
            QString str = nodeName();
            str += "(";
            if(inputs()->isNotEmpty()) str += inputs()->toString(debug, level);
            str += ")";
            if(hasCodeInputName()) str += " " + codeInputName() + "...";
            if(outputs()->isNotEmpty()) str += " -> " + outputs()->toString(debug, level);
            return str;
        }
    private:
        ParameterList *_inputs;
        ParameterList *_outputs;
        QString *_codeInputName;
    };
}

LIU_END

#endif // LIU_LANGUAGE_ABSTRACT_METHOD_H
