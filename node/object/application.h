#ifndef LIU_APPLICATION_H
#define LIU_APPLICATION_H

#include "node/object.h"

LIU_BEGIN

#define LIU_APPLICATION(ARGS...) new Application(context()->child("Object", "Application"), ##ARGS)

class Application : public Object {
    LIU_DECLARE(Application, Object, Object);
public:
    explicit Application(Node *origin) : Object(origin) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Application);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Application);
};

LIU_END

#endif // LIU_APPLICATION_H
