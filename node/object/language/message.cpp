#include "node/object/boolean.h"
#include "node/object/language/message.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Message, Object, Language);

    void Message::initRoot() {
    }

    bool Message::isDefined(QSet<const Node *> *alreadySeen) const {
        Q_UNUSED(alreadySeen);
//        return !name().isEmpty() || Object::isDefined(alreadySeen); // TODO
        return true;
    }

    Node *Message::run(Node *receiver) {
        Node *rcvr = isParented() ? receiver->parent() : receiver;
        Node *parent;
        Node *result = rcvr->child(name(), NULL, &parent);
        Alias *alias = Alias::dynamicCast(result);
        if(alias && alias->isNotEmpty())
            result = rcvr->child(alias->names(), NULL, &parent);
        if(!isEscaped()) {
            if(result->isAutoRunnable()) {
                LIU_PUSH_RUN(this);
                result = result->run(parent);
            } else if(inputs(false)) {
                Message *forkMessage = fork();
                forkMessage->setName("fork");
                result = forkMessage->run(result);
            }
        }
        return result;
    }

    QString Message::toString(bool debug, short level) const {
        QString str;
        if(isEscaped()) str += "\\";
        if(isParented()) str += "@";
        str += name();
        if(inputs(false)) str += "(" + inputs()->toString(debug, level) + ")";
        if(isQuestioned()) str += "?";
        if(isExclaimed()) str += "!";
        if(isEllipsed()) str += "...";
        if(hasCodeInput()) str += " " + codeInputName() + "...";
        if(outputs(false)) str += " -> " + outputs()->toString(debug, level);
        return str;
    }
}

LIU_END
