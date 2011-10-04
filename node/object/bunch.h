#ifndef LIU_BUNCH_H
#define LIU_BUNCH_H

#include "node/object/list.h"

LIU_BEGIN

#define LIU_BUNCH(ARGS...) new Bunch(context()->child("Object", "Bunch"), ##ARGS)

class Bunch : public GenericList<Node *> {
    LIU_DECLARE(Bunch, OldList, Object);
public:
    explicit Bunch(Node *origin) : GenericList<Node *>(origin, true) {}

    Bunch(Node *origin, Node *value) : GenericList<Node *>(origin, value, true) {}

    Bunch(Node *origin, Node *value1, Node *value2) :
        GenericList<Node *>(origin, value1, value2, true) {}

    Bunch(Node *origin, Node *value1, Node *value2, Node *value3) :
        GenericList<Node *>(origin, value1, value2, value3, true) {}

    Bunch(const Bunch &other) : GenericList<Node *>(other) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Bunch);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Bunch);

    virtual QString toString(bool debug = false, short level = 0) const {
        return "[" + join(", ", "", "", debug, level) + "]";
    }
};

LIU_END

#endif // LIU_BUNCH_H
