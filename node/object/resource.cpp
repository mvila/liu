#include "node/object/resource.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Resource, Object, Object);

Resource *Resource::init(const QString *url) {
    Object::init();
    setUrl(url);
    return this;
}

Resource *Resource::initCopy(const Resource *other) {
    Object::initCopy(other);
    setUrl(other->_url);
    return this;
}

Resource::~Resource() {
    setUrl();
}

void Resource::initRoot() {
    LIU_ADD_NATIVE_METHOD(Resource, init);

    LIU_ADD_PROPERTY(Resource, url);

    LIU_ADD_NATIVE_METHOD(Resource, get);
}

LIU_DEFINE_NATIVE_METHOD(Resource, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasInput(0)) setUrl(message->runFirstInput()->toString());
    return this;
}

LIU_DEFINE_ACCESSOR(Resource, QString, url, Url,);

LIU_DEFINE_NATIVE_METHOD(Resource, url_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(Resource::cast(parent())->hasUrl());
    else
        return Text::make(Resource::cast(parent())->url());
}

LIU_DEFINE_NATIVE_METHOD(Resource, url_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Resource::cast(parent())->setUrl(message->runFirstInput()->toString());
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Resource, get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(url().isEmpty()) LIU_THROW_RUNTIME_EXCEPTION("'url' is empty");
    QString theUrl = normalizeUrl(url());
    if(theUrl.startsWith("file://")) return Text::make(readTextFile(theUrl.mid(7)));
    LIU_THROW_RUNTIME_EXCEPTION("unknown protocol");
}

bool Resource::isEqualTo(const Node *other) const {
    const Resource *otherResource = Resource::dynamicCast(other);
    return otherResource && url() == otherResource->url();
}

QString Resource::toString(bool debug, short level) const {
    Q_UNUSED(level);
    return debug ? "\"" + url() + "\"" : url();
}

LIU_END
