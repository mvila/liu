#ifndef LIU_LANGUAGE_OPERATOR_H
#define LIU_LANGUAGE_OPERATOR_H

#include "node/object/language.h"

LIU_BEGIN

namespace Language {
    #define LIU_OPERATOR(ARGS...) \
    new Language::Operator(context()->child("Object", "Language", "Operator"), ##ARGS)

    class Operator : public Object {
        LIU_DECLARE(Operator, Object, Language);
    public:
        enum Type { Null, Prefix, Postfix, Binary };
        enum Associativity { LeftAssociative, RightAssociative, NonAssociative };
        static const short namePrecedence = 911;

        QString text;
        Type type;
        short precedence;
        Associativity associativity;
        bool useLHSAsReceiver;
        bool isSpecial;
        QString name;

        explicit Operator(Node *origin, const QString &text = "", Type type = Null, short precedence = 0,
                Associativity associativity = LeftAssociative, bool useLHSAsReceiver = true,
                 bool isSpecial = false, const QString &name = "") :
            Object(origin), text(text), type(type), precedence(precedence), associativity(associativity),
            useLHSAsReceiver(useLHSAsReceiver), isSpecial(isSpecial), name(name) {
            if(name.isEmpty()) this->name = text;
        }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Operator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Operator, text, type, precedence, associativity, useLHSAsReceiver, isSpecial, name);

        bool isNull() const { return type == Null; }

        virtual QString toString(bool debug = false, short level = 0) const {
            Q_UNUSED(debug);
            Q_UNUSED(level);
            return QString("\"%1\"").arg(name);
        }
    };
}

LIU_END

#endif // LIU_LANGUAGE_OPERATOR_H
