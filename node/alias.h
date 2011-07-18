#ifndef LIU_ALIAS_H
#define LIU_ALIAS_H

#include <QtCore/QStringList>

#include "node.h"

LIU_BEGIN

#define LIU_ALIAS(ARGS...) new Alias(context()->child("Alias"), ##ARGS)

class Alias : public Node {
    LIU_DECLARE(Alias, Node, Node);
public:
    explicit Alias(Node *origin, const QStringList &names = QStringList()) : Node(origin), _names(names) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Alias);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Alias, names());

    LIU_DECLARE_NATIVE_METHOD(init);

    QStringList &names() const { return constCast(this)->_names; }
    void setNames(const QStringList &names) { _names = names; }

    bool isEmpty() const { return names().isEmpty(); }
    bool isNotEmpty() const { return !isEmpty(); }
private:
    QStringList _names;
};

LIU_END

#endif // LIU_ALIAS_H
