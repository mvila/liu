#include "node/object/language/operatortable.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(OperatorTable, List, Language);

    void OperatorTable::initRoot() {
    }

    void OperatorTable::hasChanged() {
        _texts.clear();
        _textsAndTypes.clear();
        _firstChars.clear();
        Iterator i(this);
        while(Operator *op = i.next()) {
            _texts.insert(op->text);
            _textsAndTypes.insert(QPair<QString, Operator::Type>(op->text, op->type), op);
            if(!_firstChars.contains(op->text.at(0))) _firstChars += op->text.at(0);
        }
    }
}

LIU_END
