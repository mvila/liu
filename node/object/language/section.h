#ifndef LIU_LANGUAGE_SECTION_H
#define LIU_LANGUAGE_SECTION_H

#include "node/object/list.h"
#include "node/object/language/primitive.h"

LIU_BEGIN

namespace Language {
    #define LIU_SECTION(ARGS...) \
    new Language::Section(context()->child("Object", "Language", "Section"), ##ARGS)

    class Section : public GenericList<Primitive *> {
        LIU_DECLARE(Section, OldList, Language);
    public:
        explicit Section(Node *origin, Primitive *label = NULL) :
            GenericList<Primitive *>(origin), _label(label) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Section);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Section, LIU_FORK_IF_NOT_NULL(label()));

        Primitive *label() const { return _label; }
        void setLabel(Primitive *label) { _label = label; }

        virtual Node *run(Node *receiver = context());

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        Primitive *_label;
    };
}

LIU_END

#endif // LIU_LANGUAGE_SECTION_H
