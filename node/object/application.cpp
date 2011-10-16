#include "node/object/application.h"

LIU_BEGIN

LIU_DEFINE_2(Application, Object, Object);

Application *Application::init() {
    Object::init();
    return this;
}

Application *Application::initCopy(const Application *other) {
    Object::initCopy(other);
    return this;
}

Application::~Application() {}

void Application::initRoot() {}

LIU_END
