#include "node/object/language/section.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Section, Object, Language);

    Section *Section::init(Primitive *label, List *lines) {
        Object::init();
        setLabel(label);
        setLines(lines);
        return this;
    }

    Section *Section::initCopy(const Section *other) {
        Object::initCopy(other);
        setLabel(other->_label);
        setLines(other->_lines);
        return this;
    }

    Section::~Section() {
        setLabel();
        setLines();
    }

    void Section::initRoot() {
        setLabel(Primitive::root());
        setLines(List::root());
    }

    LIU_DEFINE_NODE_ACCESSOR(Section, Primitive, label, Label);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Section, label);

    LIU_DEFINE_NODE_ACCESSOR(Section, List, lines, Lines);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Section, lines);

    Node *Section::run(Node *receiver) {
        LIU_PUSH_RUN(this);
        Node *result = NULL;
        QScopedPointer<List::Iterator> i(lines()->iterator());
        while(i->hasNext()) result = receiver->receive(Primitive::cast(i->next().second));
        return result;
    }

    QString Section::toString(bool debug, short level) const {
        QString str;
        if(hasLabel()) str += QString("    ").repeated(level - 1) + label()->toString(debug, level) + ":";
        str = concatenateStrings(str, "\n", lines()->join("\n", QString("    ").repeated(level), "", debug, level));
        return str;
    }
}

LIU_END
