#ifndef LIU_LANGUAGE_PARAMETER_LIST_H
#define LIU_LANGUAGE_PARAMETER_LIST_H

#include "node/object/oldlist.h"
#include "node/object/language/parameter.h"

LIU_BEGIN

namespace Language {
    #define LIU_PARAMETER_LIST(ARGS...) \
    new Language::ParameterList(context()->child("Object", "Language", "ParameterList"), ##ARGS)

    class ParameterList : public GenericList<Parameter *> {
        LIU_DECLARE(ParameterList, OldList, Language);
    public:
        explicit ParameterList(Node *origin) : GenericList<Parameter *>(origin) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(ParameterList);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(ParameterList);

        using GenericList<Parameter *>::get;
        Parameter *get(const QString &label);
        bool hasLabel(const QString &label);

        QHash<QString, Parameter *> labels();

        virtual void hasChanged();

        virtual QString toString(bool debug = false, short level = 0) const {
            return join(", ", "", "", debug, level);
        }
    private:
        QHash<QString, Parameter *> _labels;
    };
}

LIU_END

#endif // LIU_LANGUAGE_PARAMETER_LIST_H
