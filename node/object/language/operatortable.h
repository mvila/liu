#ifndef LIU_LANGUAGE_OPERATOR_TABLE_H
#define LIU_LANGUAGE_OPERATOR_TABLE_H

#include <QtCore/QSet>

#include "node/object/list.h"
#include "node/object/language/operator.h"

LIU_BEGIN

namespace Language {
    #define LIU_OPERATOR_TABLE(ARGS...) \
    new Language::OperatorTable(context()->child("Object", "Language", "OperatorTable"), ##ARGS)

    class OperatorTable : public GenericList<Operator *> {
        LIU_DECLARE(OperatorTable, OldList, Language);
    public:
        explicit OperatorTable(Node *origin) : GenericList<Operator *>(origin) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(OperatorTable);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(OperatorTable);

        void append(const QString &text, Operator::Type type, short precedence,
                    Operator::Associativity associativity = Operator::LeftAssociative,
                    bool useLHSAsReceiver = true, bool isSpecial = false,
                    const QString &name = "") {
            GenericList<Operator *>::append(
                        LIU_OPERATOR(text, type, precedence, associativity, useLHSAsReceiver, isSpecial, name));
        }

        bool has(const QString &text) const {
            return _texts.contains(text);
        }

        Operator *has(const QString &text, const Operator::Type type) const {
            return _textsAndTypes.value(QPair<QString, Operator::Type>(text, type));
        }

        bool hasOperatorStartingWith(const QChar c) const {
            return _firstChars.contains(c);
        }

        virtual void hasChanged();
    private:
        QSet<QString> _texts;
        QHash<QPair<QString, Operator::Type>, Operator *> _textsAndTypes;
        QString _firstChars;
    };
}

LIU_END

#endif // LIU_LANGUAGE_OPERATOR_TABLE_H
