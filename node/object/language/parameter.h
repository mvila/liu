#ifndef LIU_LANGUAGE_PARAMETER_H
#define LIU_LANGUAGE_PARAMETER_H

#include "node/object/oldpair.h"
#include "node/object/language/primitive.h"

LIU_BEGIN

namespace Language {
    #define LIU_PARAMETER(ARGS...) \
    new Language::Parameter(context()->child("Object", "Language", "Parameter"), ##ARGS)

    class Parameter : public GenericPair<QString, Primitive *> {
        LIU_DECLARE(Parameter, OldPair, Language);
    public:
        explicit Parameter(Node *origin, const QString &label = "", Primitive *defaultValue = NULL,
                           bool isEscaped = false, bool isParented = false) :
            GenericPair<QString, Primitive *>(origin, "", NULL), _isEscaped(isEscaped), _isParented(isParented) {
            setLabel(label); setDefaultValue(defaultValue);
        }

        virtual ~Parameter() {
            setValue(NULL);
        }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Parameter);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Parameter, label(), LIU_FORK_IF_NOT_NULL(defaultValue()), isEscaped(), isParented());

        virtual void secondValueWillChange() { if(second()) removeAnonymousChild(second()); }
        virtual void secondValueHasChanged() { if(second()) addAnonymousChild(second()); }

        // aliases...
        QString label() const { return key(); }
        void setLabel(const QString &label) { setKey(label); }
        Primitive *defaultValue() const { return value(); }
        void setDefaultValue(Primitive *defaultValue) { setValue(defaultValue); }

        virtual Node *run(Node *receiver = context()) {
            // LIU_PUSH_RUN(this); // is it really necessary?
            return defaultValue()->run(receiver);
        }

        bool isEscaped() const { return _isEscaped; }
        void setIsEscaped(bool isEscaped) { _isEscaped = isEscaped; }

        bool isParented() const { return _isParented; }
        void setIsParented(bool isParented) { _isParented = isParented; }

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        bool _isEscaped;
        bool _isParented;
    };
}

LIU_END

#endif // LIU_LANGUAGE_PARAMETER_H
