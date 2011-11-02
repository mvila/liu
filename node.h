#ifndef LIU_NODE_H
#define LIU_NODE_H

#include <QtCore/QHash>

#include "liu.h"

LIU_BEGIN

#define LIU_NODE(ARGS...) new Node(context()->child("Node"), ##ARGS)

#define LIU_DECLARE_AND_DEFINE_FORK_METHOD(NAME, ARGS...) \
virtual NAME *fork() const { \
    NAME *node = new NAME(constCast(this), ##ARGS); \
    return node; \
}

#define LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(NAME, ARGS...) \
virtual NAME *fork() const { return (new NAME(constCast(this)))->init(ARGS); }

#define LIU_DECLARE_FORK_METHOD(NAME) \
virtual NAME *fork() const;

#define LIU_DEFINE_FORK_METHOD(NAME, ARGS...) \
NAME *NAME::fork() const { \
    NAME *node = new NAME(constCast(this), ##ARGS); \
    return node; \
}

#define LIU_DECLARE_AND_DEFINE_COPY_METHOD(NAME) \
virtual NAME *copy() const { return new NAME(*this); }

#define LIU_DECLARE_AND_DEFINE_COPY_METHOD_2(NAME) \
virtual NAME *copy() const { return (new NAME(this->origin()))->initCopy(this); }

#define LIU_FORK_IF_NOT_NULL(NODE) \
((NODE) ? (NODE)->fork() : NULL)

#define LIU_SET_FIELD(FIELD, VALUE) \
if((VALUE) != FIELD) { \
    if(FIELD) removeUnnamedChild(FIELD); \
    FIELD = (VALUE); \
    if(VALUE) addUnnamedChild(VALUE); \
}

#define LIU_UNSET_FIELD(FIELD) \
if(FIELD) removeUnnamedChild(FIELD);

namespace Language { class Primitive; class Message; }
using namespace Language;

#define LIU_DECLARE_NATIVE_METHOD(METHOD) \
Node *_##METHOD##_()

#define LIU_DEFINE_NATIVE_METHOD(NAME, METHOD) \
Node *NAME::_##METHOD##_()

#define LIU_DECLARE_ACCESSOR(TYPE, NAME, NAME_CAP) \
TYPE *has##NAME_CAP() const; \
TYPE NAME() const; \
void set##NAME_CAP(const TYPE *NAME = NULL); \
void set##NAME_CAP(const TYPE &NAME) { set##NAME_CAP(&NAME); }

#define LIU_DEFINE_ACCESSOR(CLASS, TYPE, NAME, NAME_CAP, DEFAULT) \
TYPE *CLASS::has##NAME_CAP() const { \
    if(_##NAME) return _##NAME; \
    if(CLASS *orig = CLASS::dynamicCast(origin())) return orig->has##NAME_CAP(); \
    return NULL; \
} \
\
TYPE CLASS::NAME() const { \
    TYPE *has = has##NAME_CAP(); \
    return has ? *has : TYPE(DEFAULT); \
} \
\
void CLASS::set##NAME_CAP(const TYPE *NAME) { \
    if(NAME) { \
        if(_##NAME) \
            *_##NAME = *NAME; \
        else \
            _##NAME = new TYPE(*NAME); \
    } else if(_##NAME) { \
        delete _##NAME; \
        _##NAME = NULL; \
    } \
    CLASS::hasChanged(); \
}

#define LIU_DECLARE_NODE_ACCESSOR(TYPE, NAME, NAME_CAP) \
TYPE *NAME() const; \
void set##NAME_CAP(TYPE *NAME = NULL); \
void delete##NAME_CAP(); \
virtual void NAME##WillChange(); \
virtual void NAME##HasChanged();

#define LIU_DEFINE_NODE_ACCESSOR(CLASS, TYPE, NAME, NAME_CAP) \
TYPE *CLASS::NAME() const { \
    if(_##NAME) return _##NAME; \
    CLASS *orig = CLASS::dynamicCast(origin()); \
    if(!orig) LIU_THROW_NULL_POINTER_EXCEPTION(QString("'%1' is undefined").arg(#NAME)); \
    TYPE *result = orig->NAME()->fork(); \
    result->setIsVirtual(true); \
    constCast(this)->_##NAME = result; \
    constCast(this)->addUnnamedChild(result); \
    return result; \
} \
void CLASS::set##NAME_CAP(TYPE *NAME) { \
    NAME##WillChange(); \
    if(_##NAME) removeUnnamedChild(_##NAME); \
    _##NAME = NAME; \
    if(NAME) addUnnamedChild(NAME); \
    NAME##HasChanged(); \
    CLASS::hasChanged(); \
} \
void CLASS::delete##NAME_CAP() { \
    NAME##WillChange(); \
    if(_##NAME) { \
        removeUnnamedChild(_##NAME); \
        delete _##NAME; \
        _##NAME = NULL; \
    } \
    NAME##HasChanged(); \
    CLASS::hasChanged(); \
}

#define LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(CLASS, NAME) \
void CLASS::NAME##WillChange() {} \
void CLASS::NAME##HasChanged() {}

#define LIU_CHECK_POINTER(POINTER) \
if(!(POINTER)) LIU_THROW_NULL_POINTER_EXCEPTION("Node pointer is NULL")

class Node {
public:
    friend class NamedChildDictionary;

    typedef Node *NodePtr;

    static const bool isInitialized;

    explicit Node(Node *origin) : _origin(origin), _extensions(NULL), // default constructor
        _children(NULL), _parents(NULL), _isDefined(false), _isUndefined(false), _isVirtual(false), _isAutoRunnable(false) { initFork(); }

    Node(const Node &other); // copy constructor

    Node *init();
    Node *initCopy(const Node *other);

    void initFork();

    virtual ~Node();

    inline static Node *constCast(const Node *node) { return const_cast<Node *>(node); }

    static Node *root();
    virtual void initRoot();

    const QString nodeName() const;
    void setNodeName(const QString &name);
    const QString nodePath() const;

    void declare(const QString &name) const;

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Node);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Node);

    LIU_DECLARE_NATIVE_METHOD(copy);

    LIU_DECLARE_NATIVE_METHOD(fork);
    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_NATIVE_METHOD(self);

    Node *origin() const {
        if(!_origin) LIU_THROW_NULL_POINTER_EXCEPTION("origin is NULL");
        return _origin;
    }

    Node *hasOrigin() const {
        return _origin && _origin != this ? _origin : NULL;
    }

    LIU_DECLARE_NATIVE_METHOD(origin_get);

    void setOrigin(Node *node);
    LIU_DECLARE_NATIVE_METHOD(origin_set);

    bool isOriginatingFrom(Node *node) const;
    LIU_DECLARE_NATIVE_METHOD(is);

    virtual bool isDefined(QSet<const Node *> *alreadySeen = NULL) const;
    void setIsDefined(bool isDefined) { _isDefined = isDefined; }
    void setIsUndefined(bool isUndefined) { _isUndefined = isUndefined; }

    LIU_DECLARE_NATIVE_METHOD(defined);
    LIU_DECLARE_NATIVE_METHOD(undefined);
    LIU_DECLARE_NATIVE_METHOD(define);
    LIU_DECLARE_NATIVE_METHOD(undefine);

    bool isVirtual() const { return _isVirtual; }
    bool isReal() const { return !_isVirtual; }
    void setIsVirtual(bool isVirtual) { _isVirtual = isVirtual; }
    void setIsReal(bool isReal) { _isVirtual = !isReal; }

    Node *virtualOrReal(bool virtualMode);
    LIU_DECLARE_NATIVE_METHOD(virtual) { return virtualOrReal(true); }
    LIU_DECLARE_NATIVE_METHOD(real) { return virtualOrReal(false); }

    bool isAutoRunnable() const { return _isAutoRunnable; }
    void setIsAutoRunnable(bool isAutoRunnable) { _isAutoRunnable = isAutoRunnable; }

    Node *real();
    const Node *real() const { return constCast(this)->real(); }

    void addExtension(Node *node);
    void prependExtension(Node *node);
    void removeExtension(Node *node);
    void removeAllExtensions();
    bool hasExtension(Node *node) const;
    QList<Node *> extensions() const;

    LIU_DECLARE_NATIVE_METHOD(extensions_get);

    Node *child(const QString &name, bool *wasFoundPtr = NULL, Node **parentPtr = NULL) const;

    Node *child(const QStringList &names, bool *wasFoundPtr = NULL, Node **parentPtr = NULL) const;

    Node *child(const QString &name1, const QString &name2) const {
        return child(name1)->child(name2);
    }

    Node *child(const QString &name1, const QString &name2, const QString &name3) const {
        return child(name1)->child(name2)->child(name3);
    }

    Node *addChild(const QString &name, Node *value);
    Node *setChild(const QString &name, Node *value, bool addOrSetMode = false);
private:
    void _setChild(const QString &name, Node *value);
public:
    Node *addOrSetChild(const QString &name, Node *value) { return setChild(name, value, true); }

private:
    Node *defineOrAssign(bool isDefine);
public:
    LIU_DECLARE_NATIVE_METHOD(define_or_assign) { return defineOrAssign(true); }
    LIU_DECLARE_NATIVE_METHOD(assign) { return defineOrAssign(false); }

    virtual void hasBeenDefined(Message *message);

    void removeChild(const QString &name, bool *wasFoundPtr = NULL);

    LIU_DECLARE_NATIVE_METHOD(remove);

    void addUnnamedChild(Node *value) {
        LIU_CHECK_POINTER(value);
        value->_addParent(this);
    }

    void removeUnnamedChild(Node *value) {
        LIU_CHECK_POINTER(value);
        value->_removeParent(this);
    }

    Node *findChild(const QString &name, bool searchInParents = true, Node **parentPtr = NULL,
                   bool autoFork = true, bool *isDirectPtr = NULL) const;
private:
    Node *findChildInSelfOrOrigins(const QString &name, bool autoFork = true, bool *isDirectPtr = NULL) const;
public:
    bool hasChild(const QString &name, bool searchInParents = true) const {
        return findChild(name, searchInParents);
    }

    LIU_DECLARE_NATIVE_METHOD(has);

    Node *hasDirectChild(const QString &name, bool *isRemovedPtr = NULL) const {
        Node *child = NULL;
        if(_children) child = _children->value(name);
        if(isRemovedPtr) *isRemovedPtr = !child && _children && _children->contains(name);
        return child;
    }

    QString hasDirectChild(Node *value) const {
        return _children ? _children->key(value) : QString();
    }

    bool hasDirectParent(Node *parent) const {
        LIU_CHECK_POINTER(parent);
        return _parents && _parents->contains(parent);
    }
private:
    void _addParent(Node *parent) const;
    void _removeParent(Node *parent) const;
public:
    LIU_DECLARE_NATIVE_METHOD(children);

    virtual Node *unnamedChild(int index) const {
        Q_UNUSED(index);
        return NULL;
    }

    Node *parent() const;
    bool hasOneParent() const;
    LIU_DECLARE_NATIVE_METHOD(parent);

    Node *findParentOriginatingFrom(Node *orig) const;

    virtual Node *receive(Primitive *primitive);

    virtual Node *run(Node *receiver = context()) {
        Q_UNUSED(receiver);
        return this;
    }

    LIU_DECLARE_NATIVE_METHOD(or);
    LIU_DECLARE_NATIVE_METHOD(and);
    LIU_DECLARE_NATIVE_METHOD(not);

    LIU_DECLARE_NATIVE_METHOD(or_assign);
    LIU_DECLARE_NATIVE_METHOD(and_assign);

    bool isSameAs(const Node *other) const { return real() == other->real(); }
    LIU_DECLARE_NATIVE_METHOD(same_as);

    virtual bool isEqualTo(const Node *other) const { return isSameAs(other); }
    LIU_DECLARE_NATIVE_METHOD(equal_to);
    LIU_DECLARE_NATIVE_METHOD(different_from);

    virtual short compare(const Node *other) const;

    LIU_DECLARE_NATIVE_METHOD(throw);

    static HugeUnsignedInteger &passedAssertionCount() {
        static HugeUnsignedInteger _count = 0;
        return _count;
    }

    Node *assert(bool isAssertTrue = true);
    LIU_DECLARE_NATIVE_METHOD(assert_true) { return assert(true); }
    LIU_DECLARE_NATIVE_METHOD(assert_false) { return assert(false); }

    virtual void hasChanged() {}

    void inspect() const { P(toString(true).toUtf8()); } // TODO: use Console print
    virtual void inspectInternal() const;
    LIU_DECLARE_NATIVE_METHOD(inspect);

    void dump() const { P(Node::toString(true).toUtf8()); } // TODO: use Console print
    LIU_DECLARE_NATIVE_METHOD(dump);

    long long int memoryAddress() const { return reinterpret_cast<long long int>(this); }
    LIU_DECLARE_NATIVE_METHOD(memory_address);

    QString hexMemoryAddress() const { return QString("0x%1").arg(memoryAddress(), 0, 16); }

    virtual bool toBool() const { return isDefined(); };

    virtual double toDouble(bool *okPtr = NULL) const {
        if(okPtr)
            *okPtr = false;
        else
            LIU_THROW_CONVERSION_EXCEPTION(QString("cannot convert from %1 to Number").arg(nodeName()));
        return 0;
    };

    virtual QChar toChar(bool *okPtr = NULL) const {
        if(okPtr)
            *okPtr = false;
        else
            LIU_THROW_CONVERSION_EXCEPTION(QString("cannot convert from %1 to Character").arg(nodeName()));
        return 0;
    };

    virtual QString toString(bool debug = false, short level = 0) const;

    virtual uint hash() const { return ::qHash(real()); }
    LIU_DECLARE_NATIVE_METHOD(hash);
private:
    Node *_origin;
    QList<Node *> *_extensions;
    QHash<QString, Node *> *_children;
    mutable QHash<Node *, HugeUnsignedInteger> *_parents;
    bool _isDefined      : 1;
    bool _isUndefined    : 1;
    bool _isVirtual      : 1;
    bool _isAutoRunnable : 1;
public:
    // === Reference ===

    class Reference {
    public:
        Reference(Node *node) : _node(node) {}
        Node &operator*() { return *_node; }
        const Node &operator*() const { return *_node; }
        Node *operator->() { return _node; }
        const Node *operator->() const { return _node; }
        operator bool() const { return _node; }
        bool operator!() const { return !_node; }
        operator Node *() const { return _node; }
    private:
        Node *_node;
    };
};

typedef QPair<Node *, Node *> NodeQPair;

inline bool operator==(const Node &a, const Node &b) { return a.isEqualTo(&b); }
inline bool operator!=(const Node &a, const Node &b) { return !a.isEqualTo(&b); }
inline uint qHash(const Node &node) { return node.hash(); }

inline bool operator==(const Node::Reference &a, const Node::Reference &b) { return a->isEqualTo(b); }
inline bool operator!=(const Node::Reference &a, const Node::Reference &b) { return !a->isEqualTo(b); }
inline uint qHash(const Node::Reference &node) { return node->hash(); }

#define LIU_DECLARE(NAME, ORIGIN, PARENT) \
public: \
    inline static NAME *cast(Node *node) { return static_cast<NAME *>(node); } \
    inline static const NAME *cast(const Node *node) { return static_cast<const NAME *>(node); } \
    inline static NAME *dynamicCast(Node *node) { return dynamic_cast<NAME *>(node); } \
    inline static const NAME *dynamicCast(const Node *node) { return dynamic_cast<const NAME *>(node); } \
    inline static NAME *constCast(const NAME *node) { return const_cast<NAME *>(node); } \
    static NAME *root(); \
    virtual void initRoot(); \
    static const bool isInitialized; \
    typedef NAME *NAME##Ptr; \
private:

#define LIU_DECLARE_2(NAME, ORIGIN, PARENT) \
public: \
    inline static NAME *cast(Node *node) { return static_cast<NAME *>(node); } \
    inline static const NAME *cast(const Node *node) { return static_cast<const NAME *>(node); } \
    inline static NAME *dynamicCast(Node *node) { return dynamic_cast<NAME *>(node); } \
    inline static const NAME *dynamicCast(const Node *node) { return dynamic_cast<const NAME *>(node); } \
    inline static NAME *constCast(const NAME *node) { return const_cast<NAME *>(node); } \
    static NAME *root(); \
    virtual void initRoot(); \
    static const bool isInitialized; \
    typedef NAME *NAME##Ptr; \
    static NAME *make() { return (new NAME())->init(); } \
    NAME *initCopy(const NAME *other); \
    virtual ~NAME(); \
    virtual NAME *fork() const { return (new NAME(constCast(this)))->init(); } \
    virtual NAME *copy() const { return (new NAME(this->origin()))->initCopy(this); } \
private:

#define LIU_DEFINE(NAME, ORIGIN, PARENT) \
const bool NAME::isInitialized = NAME::root(); \
NAME *NAME::root() { \
    static NAME *_root = NULL; \
    if(!_root) { \
        _root = new NAME(ORIGIN::root()); \
        QString name = #NAME; \
        int pos = name.lastIndexOf("::"); \
        if(pos != -1) name = name.mid(pos + 2); \
        PARENT::root()->addOrSetChild(name, _root); \
        _root->declare(name); \
    } \
    return _root; \
}

#define LIU_DEFINE_2(NAME, ORIGIN, PARENT) \
const bool NAME::isInitialized = NAME::root(); \
NAME *NAME::root() { \
    static NAME *_root = NULL; \
    if(!_root) { \
        _root = new NAME(ORIGIN::root()); \
        _root->init(); \
        QString name = #NAME; \
        int pos = name.lastIndexOf("::"); \
        if(pos != -1) name = name.mid(pos + 2); \
        PARENT::root()->addOrSetChild(name, _root); \
        _root->declare(name); \
    } \
    return _root; \
}

LIU_END

#endif // LIU_NODE_H
