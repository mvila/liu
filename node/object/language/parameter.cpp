#include "node/object/language/parameter.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Parameter, OldPair, Language);

    void Parameter::initRoot() {
    }

    QString Parameter::toString(bool debug, short level) const {
        QString prefixes;
        if(isEscaped())
            prefixes += "\\";
        if(isParented())
            prefixes += "@";
        return concatenateStrings(prefixes + label(), ": ",
                                  defaultValue() ? defaultValue()->toString(debug, level) : "");
    }
}

LIU_END
