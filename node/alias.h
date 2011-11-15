#ifndef LIU_ALIAS_H
#define LIU_ALIAS_H

#include <QtCore/QStringList>

#include "node.h"

LIU_BEGIN

class Alias : public Node {
    LIU_DECLARE_2(Alias, Node, Node);
public:
    explicit Alias(Node *origin = context()->child("Alias")) :
        Node(origin), _names(NULL) {}

    static Alias *make(const QStringList &names) { return (new Alias())->init(&names); }

    Alias *init(const QStringList *names = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(QStringList, names, Names);

    bool isEmpty() const { return names().isEmpty(); }
    bool isNotEmpty() const { return !isEmpty(); }
private:
    QStringList *_names;
};

LIU_END

#endif // LIU_ALIAS_H
