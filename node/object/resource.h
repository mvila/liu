#ifndef LIU_RESOURCE_H
#define LIU_RESOURCE_H

#include "node/object/boolean.h"
#include "node/object/text.h"
#include "node/object/property.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Resource : public Object {
    LIU_DECLARE_2(Resource, Object, Object);
public:
    explicit Resource(Node *origin = context()->child("Object", "Resource")) :
        Object(origin), _url(NULL) {}

    static Resource *make(const QString &url) { return (new Resource())->init(&url); }

    Resource *init(const QString *url = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(QString, url, Url);
    LIU_DECLARE_PROPERTY(url);

    LIU_DECLARE_NATIVE_METHOD(get);

    virtual bool isEqualTo(const Node *other) const;

    virtual QString toString(bool debug = false, short level = 0) const;
private:
    QString *_url;
};

LIU_END

#endif // LIU_RESOURCE_H
