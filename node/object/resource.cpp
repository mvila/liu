#include "node/object/resource.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Resource ===

LIU_DEFINE(Resource, Object, Object);

Resource::Resource(Node *origin, const QString &url) : Object(origin) {
    if(!url.isNull()) setUrl(url);
}

void Resource::initRoot() {
    LIU_ADD_NATIVE_METHOD(Resource, init);

    LIU_ADD_NATIVE_METHOD(Resource, equal_to, ==);

    LIU_ADD_NATIVE_METHOD(Resource, get);
}

LIU_DEFINE_NATIVE_METHOD(Resource, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasInput(0)) setUrl(message->runFirstInput()->toString());

//    // === TODO: DRY ===
//    LIU_FIND_LAST_PRIMITIVE;
//    Primitive *nextPrimitive = primitive->next();
//    if(nextPrimitive) {
//        nextPrimitive->run(this);
//        Primitive::skip(this);
//    }

    return this;
}

LIU_DEFINE_NATIVE_METHOD(Resource, get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(url().isEmpty()) LIU_THROW_RUNTIME_EXCEPTION("'url' is empty");
    QString theUrl = normalizeUrl(url());
    if(theUrl.startsWith("file://")) return LIU_TEXT(readTextFile(theUrl.mid(7)));
    LIU_THROW_RUNTIME_EXCEPTION("unknown protocol");
}

bool Resource::isEqualTo(const Node *other) const {
    const Resource *otherResource = Resource::dynamicCast(other);
    return otherResource && url() == otherResource->url();
}

LIU_DEFINE_NATIVE_METHOD(Resource, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(url() == message->runFirstInput()->toString());
}

QString Resource::toString(bool debug, short level) const {
    Q_UNUSED(level);
    return debug ? "\"" + url() + "\"" : url();
}

LIU_END
