#include "node/object/language/parameterlist.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(ParameterList, List, Language);

    void ParameterList::initRoot() {
    }

    void ParameterList::hasChanged() {
        _labels.clear();
        Iterator i(this);
        while(Parameter *parameter = i.next()) {
            if(_labels.contains(parameter->label()))
                LIU_THROW(DuplicateException, "duplicated label found in parameter list");
            _labels.insert(parameter->label(), parameter);
        }
    }
}

LIU_END
