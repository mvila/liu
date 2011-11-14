#include "node/object/language/abstractmethod.h"
#include "node/object/language/message.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(AbstractMethod, Object, Language);

    void AbstractMethod::initRoot() {
    }

    void AbstractMethod::runParameters() {
        return;
        LIU_FIND_LAST_MESSAGE;
//        P(message->name());
//        P(inputs()->size());
//        P(inputs()->toString());
        AbstractMethod *forkedMethod = this; //->fork();
        QHash<QString, Parameter *> labels(inputs()->labels());
        if(message->inputs(false)) {
            ArgumentBunch::Iterator iterator(message->inputs());
            short i = -1;
            bool labelFound = false;
            while(Argument *argument = iterator.next()) {
                Parameter *parameter;
                if(argument->label()) {
                    QString labelName = argument->labelName();
                    if(!hasInput(labelName)) LIU_THROW(NotFoundException, QString("unknown parameter label '%1'").arg(labelName));
                    if(!labels.contains(labelName)) LIU_THROW(DuplicateException, QString("duplicated parameter label '%1'").arg(labelName));
                    parameter = input(labelName);
                    labelFound = true;
                } else {
                    if(labelFound) LIU_THROW(ArgumentException, "positional arguments are forbidden after labeled ones");
                    i++;
                    if(!hasInput(i)) LIU_THROW(IndexOutOfBoundsException, "too many arguments");
                    parameter = input(i);
                }
                Node *rcvr = parameter->isParented() ? forkedMethod->parent() : forkedMethod;
                Node *val = parameter->isEscaped() ? argument->value() : argument->run();
                rcvr->addOrSetChild(parameter->label(), val);
                labels.remove(parameter->label());
            }
        }
        foreach(Parameter *parameter, labels) {
            if(!parameter->defaultValue()) LIU_THROW(ArgumentException, "missing mandatory parameter");
            Node *rcvr = parameter->isParented() ? forkedMethod->parent() : forkedMethod;
            Node *val = parameter->isEscaped() ? parameter->defaultValue() : parameter->run();
            rcvr->addOrSetChild(parameter->label(), val);
        }
        if(hasCodeInput()) {
            LIU_FIND_LAST_PRIMITIVE;
            Primitive *nextPrimitive = primitive->hasNext();
            forkedMethod->addOrSetChild(codeInputName(), nextPrimitive ? nextPrimitive : Primitive::make()); // FIXME: abstract primitive?
        }
    }
}

LIU_END
