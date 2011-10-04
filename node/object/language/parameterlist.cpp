#include "node/object/language/parameterlist.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(ParameterList, OldList, Language);

    void ParameterList::initRoot() {
    }

    Parameter *ParameterList::get(const QString &label) {
        hasChanged(); // FIXME: should be not necessary!
        return _labels.value(label);
    }

    bool ParameterList::hasLabel(const QString &label) {
        hasChanged(); // FIXME: should be not necessary!
        return _labels.contains(label);
    }

    QHash<QString, Parameter *> ParameterList::labels() {
        hasChanged(); // FIXME: should be not necessary!
        return _labels;
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
