#ifndef LIU_LANGUAGE_SECTION_H
#define LIU_LANGUAGE_SECTION_H

#include "node/object/list.h"
#include "node/object/language/primitive.h"

LIU_BEGIN

namespace Language {
    class Section : public Object {
        LIU_DECLARE_2(Section, Object, Language);
    public:
        explicit Section(Node *origin = context()->child("Object", "Language", "Section")) :
            Object(origin), _label(NULL), _lines(NULL) {}

        Section *init(Primitive *label = NULL, List *lines = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Primitive, label, Label);

        LIU_DECLARE_NODE_ACCESSOR(List, lines, Lines);

        virtual Node *run(Node *receiver = context());

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        Primitive *_label;
        List *_lines;
    };
}

LIU_END

#endif // LIU_LANGUAGE_SECTION_H
