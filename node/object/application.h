#ifndef LIU_APPLICATION_H
#define LIU_APPLICATION_H

#include "node/object.h"

LIU_BEGIN

class Application : public Object {
    LIU_DECLARE_2(Application, Object, Object);
public:
    explicit Application(Node *origin = context()->child("Object", "Application")) : Object(origin) {}

    Application *init();
};

LIU_END

#endif // LIU_APPLICATION_H
