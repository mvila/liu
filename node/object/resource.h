#ifndef LIU_RESOURCE_H
#define LIU_RESOURCE_H

#include "node/object/message.h"
#include "node/object/boolean.h"

LIU_BEGIN

#define LIU_RESOURCE(ARGS...) new Resource(context()->child("Object", "Resource"), ##ARGS)

class Resource : public Object {
    LIU_DECLARE(Resource, Object, Object);
public:
    explicit Resource(Node *origin, const QString &url = "");

    Resource(const Resource &other);

    virtual ~Resource() {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Resource);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Resource, url());

    LIU_DECLARE_NATIVE_METHOD(init);

    const QString &url() const { return _url; }
    void setUrl(const QString &url) { _url = url; }

    virtual bool isEqualTo(const Node *other) const;
    LIU_DECLARE_NATIVE_METHOD(equal_to);

    virtual QString toString(bool debug = false, short level = 0) const;
private:
    QString _url;
};

LIU_END

#endif // LIU_RESOURCE_H
