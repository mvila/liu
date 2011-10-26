#ifndef LIU_PAIR_H
#define LIU_PAIR_H

#include "node/object.h"
#include "node/object/property.h"

LIU_BEGIN

class Pair : public Object {
    LIU_DECLARE_2(Pair, Object, Object);
public:
    explicit Pair(Node *origin = context()->child("Object", "Pair")) :
        Object(origin), _first(NULL), _second(NULL) {}

    static Pair *make(Node *first, Node *second) { return (new Pair())->init(first, second); }

    Pair *init(Node *first = NULL, Node *second = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_NODE_ACCESSOR(Node, first, First);
    LIU_DECLARE_PROPERTY(first);

    LIU_DECLARE_NODE_ACCESSOR(Node, second, Second);
    LIU_DECLARE_PROPERTY(second);

    virtual Node *unnamedChild(int index) const;

    virtual bool isEqualTo(const Node *other) const;

    virtual short compare(const Node *other) const;

    virtual QString toString(bool debug = false, short level = 0) const {
        return QString("%1: %2").arg(first()->toString(debug, level), second()->toString(debug, level));
    }
private:
    Node *_first;
    Node *_second;
};

LIU_END

#endif // LIU_PAIR_H
