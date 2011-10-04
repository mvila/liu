#include "node/object/language/section.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Section, OldList, Language);

    void Section::initRoot() {
    }

    Node *Section::run(Node *receiver) {
        LIU_PUSH_RUN(this);
        Node *result = NULL;
        Iterator i(this);
        while(Primitive *primitive = i.next())
            result = receiver->receive(primitive);
        return result;
    }

    QString Section::toString(bool debug, short level) const {
        QString str;
        if(label()) str += QString("    ").repeated(level - 1) + label()->toString(debug, level) + ":";
        str = concatenateStrings(str, "\n", join("\n", QString("    ").repeated(level), "", debug, level));
        return str;
    }
}

LIU_END
