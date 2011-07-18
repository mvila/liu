#include "node/object/message.h"
#include "node/object/language/argument.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Argument, Pair, Language);

    void Argument::initRoot() {
    }

    QString Argument::labelName() const {
        if(label()->hasNext()) LIU_THROW(ArgumentException, "illegal parameter label");
        Message *labelMessage = Message::dynamicCast(label()->value());
        if(!labelMessage) LIU_THROW(ArgumentException, "illegal parameter label");
        return labelMessage->name();
    }

    QString Argument::toString(bool debug, short level) const {
        return concatenateStrings(label() ? label()->toString(debug, level) + ":" : "",
                                  " ",
                                  value() ? value()->toString(debug, level) : "");
    }
}

LIU_END
