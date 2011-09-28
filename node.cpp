#include <QtCore/QStringList>

#include "node.h"
#include "node/exception.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/text.h"
#include "node/object/property.h"
#include "node/object/language/message.h"
#include "node/object/language/block.h"
#include "node/object/language/nativemethod.h"
#include "node/object/language/method.h"

LIU_BEGIN

const bool Node::isInitialized = Node::root();

Node::Node(const Node &other) : _origin(other._origin), _extensions(NULL), // copy constructor
    _children(NULL), _parents(NULL), _isAbstract(other._isAbstract),
    _isVirtual(other._isVirtual), _isAutoRunnable(other._isAutoRunnable) {
    if(other._extensions) _extensions = new QList<Node *>(*other._extensions);
    if(other._children) {
        QHashIterator<QString, Node *> i(other.children());
        while(i.hasNext()) { i.next(); addOrSetChild(i.key(), i.value()); }
    }
}

void Node::initFork() {
    if(hasOrigin()) setIsAutoRunnable(origin()->isAutoRunnable());
}

Node::~Node() {
    if(_extensions) delete _extensions;
    if(_children) {
        foreach(Node *child, *_children) if(child) child->_removeParent(this);
        delete _children;
    }
    if(_parents) delete _parents;
}

Node *Node::root() {
    static Node *_root = NULL;
    if(!_root) {
        _root = new Node(NULL);
        _root->setOrigin(_root);
        _root->addChild("Node", _root);
        _root->declare("Node");
    }
    return _root;
}

void Node::initRoot() {
    pushContext(this);

    LIU_ADD_NATIVE_METHOD(Node, self);

    Property *originProperty = LIU_PROPERTY();
    originProperty->LIU_ADD_NATIVE_METHOD(Node, origin_get, get);
    originProperty->LIU_ADD_NATIVE_METHOD(Node, origin_set, set);
    addChild("origin", originProperty);

    Property *extensionsProperty = LIU_PROPERTY();
    extensionsProperty->LIU_ADD_NATIVE_METHOD(Node, extensions_get, get);
    addChild("extensions", extensionsProperty);

    LIU_ADD_NATIVE_METHOD(Node, is);

    LIU_ADD_NATIVE_METHOD(Node, virtual);
    LIU_ADD_NATIVE_METHOD(Node, real);

    LIU_ADD_NATIVE_METHOD(Node, copy);

    LIU_ADD_NATIVE_METHOD(Node, fork);
    LIU_ADD_NATIVE_METHOD(Node, init);

    LIU_ADD_NATIVE_METHOD(Node, define, :=);
    LIU_ADD_NATIVE_METHOD(Node, assign, =);
    LIU_ADD_NATIVE_METHOD(Node, remove, >>);

    LIU_ADD_NATIVE_METHOD(Node, has);

    LIU_ADD_NATIVE_METHOD(Node, parent);

    LIU_ADD_NATIVE_METHOD(Node, or, ||);
    LIU_ADD_NATIVE_METHOD(Node, and, &&);
    LIU_ADD_NATIVE_METHOD(Node, not, !);

    LIU_ADD_NATIVE_METHOD(Node, or_assign, ||=);
    LIU_ADD_NATIVE_METHOD(Node, and_assign, &&=);

    LIU_ADD_NATIVE_METHOD(Node, equal_to, ==);
    LIU_ADD_NATIVE_METHOD(Node, different_from, !=);

    LIU_ADD_NATIVE_METHOD(Node, throw);

    LIU_ADD_NATIVE_METHOD(Node, assert_true, ?:);
    LIU_ADD_NATIVE_METHOD(Node, assert_false, !:);

    LIU_ADD_NATIVE_METHOD(Node, inspect);
    LIU_ADD_NATIVE_METHOD(Node, dump);

    LIU_ADD_NATIVE_METHOD(Node, memory_address);
}

const QString Node::nodeName() const {
    return child("__name__")->toString();
}

void Node::setNodeName(const QString &name) {
    addOrSetChild("__name__", (new Text(Text::root()))->init(&name));
}

const QString Node::nodePath() const {
    QString path;
    const Node *cur = this;
    const Node *par;
    while((par = cur->parent()) != cur) {
        path.prepend(par->nodeName().toLower() + "/");
        cur = par;
    }
    return path;
}

void Node::declare(const QString &name) const {
    roots().append(Root(constCast(this), name));
}

LIU_DEFINE_NATIVE_METHOD(Node, copy) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return copy();
}

LIU_DEFINE_NATIVE_METHOD(Node, fork) {
    LIU_FIND_LAST_MESSAGE;
    Node *node = fork();
    if(node->hasChild("init")) {
        Message* initMessage = message->fork();
        initMessage->setName("init");
        initMessage->run(node);
    }
    return node;
}

LIU_DEFINE_NATIVE_METHOD(Node, init) {
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->next();
    if(nextPrimitive && Block::dynamicCast(nextPrimitive->value())) {
        nextPrimitive->run(this);
        Primitive::skip(this);
    }
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Node, self) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Node, origin_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return parent()->origin();
}

LIU_DEFINE_NATIVE_METHOD(Node, origin_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    parent()->setOrigin(message->runFirstInput());
    return this;
}

void Node::setOrigin(Node *node) {
    LIU_CHECK_POINTER(node);
    _origin = node;
}

bool Node::isOriginatingFrom(Node *orig) const {
    orig = orig->real();
    const Node *node = real();
    while(node != orig) {
        if(node->origin() == node) // Node::root reached?
            return false;
        if(_extensions) {
            foreach(Node *extension, *_extensions)
                if(extension->isOriginatingFrom(orig)) return true;
        }
        node = node->origin();
    }
    return true;
}

LIU_DEFINE_NATIVE_METHOD(Node, is) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(isOriginatingFrom(message->runFirstInput()));
}

Node *Node::virtualOrReal(bool virtualMode) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_BOOLEAN(isVirtual() == virtualMode);
}

Node *Node::real() {
    Node *node = this;
    while(node->isVirtual()) node = node->origin();
    return node;
}

void Node::addExtension(Node *node) {
    LIU_CHECK_POINTER(node);
    if(!_extensions) { _extensions = new QList<Node *>; }
    if(hasExtension(node)) LIU_THROW(DuplicateException, "cannot add an extension which is already there");
    _extensions->append(node);
}

void Node::prependExtension(Node *node)  {
    LIU_CHECK_POINTER(node);
    if(!_extensions) { _extensions = new QList<Node *>; }
    if(hasExtension(node)) LIU_THROW(DuplicateException, "cannot add an extension which is already there");
    _extensions->prepend(node);
}

void Node::removeExtension(Node *node)  {
    LIU_CHECK_POINTER(node);
    if(!hasExtension(node)) LIU_THROW(NotFoundException, "cannot remove an extension which is not there");
    _extensions->removeOne(node);
}

void Node::removeAllExtensions() {
    _extensions->clear();
}

bool Node::hasExtension(Node *node) const {
    LIU_CHECK_POINTER(node);
    return _extensions && _extensions->contains(node);
}

QList<Node *> Node::extensions() const {
    return _extensions ? *_extensions : QList<Node *>();
}

LIU_DEFINE_NATIVE_METHOD(Node, extensions_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    VirtualList *value = VirtualList::dynamicCast(hasDirectChild("cached_value"));
    if(!value) {
        value = LIU_VIRTUAL_LIST(&parent()->_extensions);
        value->setAreDuplicateValuesNotAllowed(true);
        addOrSetChild("cached_value", value);
    }
    return value;
}

Node *Node::child(const QString &name, bool *wasFoundPtr, Node **parentPtr) const {
    Node *node = findChild(name, true, parentPtr);
    if(wasFoundPtr)
        *wasFoundPtr = node;
    else if(!node)
        LIU_THROW(NotFoundException, QString("child '%1' not found").arg(name));
    return node;
}

Node *Node::child(const QStringList &names, bool *wasFoundPtr, Node **parentPtr) const {
    Node *node = constCast(this);
    foreach(QString name, names) {
        node = node->child(name, wasFoundPtr, parentPtr);
        if(wasFoundPtr && !*wasFoundPtr) break;
    }
    return node;
}

Node *Node::addChild(const QString &name, Node *value) {
    LIU_CHECK_POINTER(value);
    if(findChild(name, false)) LIU_THROW(DuplicateException, "child with same name is already there");
    _setChild(name, value);
    return value;
}

Node *Node::setChild(const QString &name, Node *value, bool addOrSetMode) {
    LIU_CHECK_POINTER(value);
    Node *parent = this;
    bool isDirect;
    if(Node *current = findChild(name, !addOrSetMode, &parent, false, &isDirect)) {
        if(isDirect) {
            if(current == value) return value;
            current->_removeParent(parent);
        }
    } else if(!addOrSetMode) LIU_THROW(NotFoundException, QString("child '%1' not found").arg(name));
    parent->_setChild(name, value);
    return value;
}

void Node::_setChild(const QString &name, Node *value) {
    if(!_children) _children = new QHash<QString, Node *>;
    _children->insert(name, value);
    if(value) value->_addParent(this);
}

Node *Node::defineOrAssign(bool isDefine) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Message *msg = Message::dynamicCast(message->firstInput()->value()->value());
    if(!msg) LIU_THROW(ArgumentException, "left-hand side is not a message");
//    if(msg->name() == "[]") {
//        message = message->fork();
//        message->setName(isDefine ? "[]:=" : "[]=");
//        if(msg->hasAnInput())
//            message->inputs()->set(0, msg->firstInput());
//        else
//            message->inputs()->remove(0);
//        if(msg->isQuestioned()) message->setIsQuestioned(true);
//        if(msg->isExclaimed()) message->setIsExclaimed(true);
//        return message->run(this);
//    }
    Node *value;
    Block *block = Block::dynamicCast(message->secondInput()->value()->value());
    if(block) { // if rhs is a block, we have a method definition shorthand
        value = LIU_METHOD(message->secondInput()->value(), NULL, NULL, "");
    } else // rhs is not a block
        value = message->runSecondInput();
    Property *property = NULL;
    if(!isDefine && (property = Property::dynamicCast(findChild(msg->name()))))
        LIU_MESSAGE("set", value)->run(property);
    else
        setChild(msg->name(), value, isDefine);
    if(isDefine) value->hasBeenDefined(msg);
    return value;
}

void Node::hasBeenDefined(Message *message) {
    if(!message->name().isEmpty() && message->name().at(0).isUpper()) {
        setNodeName(message->name());
        if(message->hasAnInput()) {
            Method *method = Method::dynamicCast(hasDirectChild("init"));
            if(!method) {
                method = LIU_METHOD();
                method->setIsAutoRunnable(true);
                method->setNodeName("init");
                addOrSetChild("init", method);
            }
            method->setInputs(message->inputs());
        }
    }
}

void Node::removeChild(const QString &name, bool *wasFoundPtr) {
    bool isDirect;
    Node *current = findChild(name, false, NULL, false, &isDirect);
    if(current) {
        if(isDirect) current->_removeParent(this);
        _setChild(name, NULL);
    }
    if(wasFoundPtr)
        *wasFoundPtr = current;
    else if(!current)
        LIU_THROW(NotFoundException, QString("child '%1' not found").arg(name));
}

LIU_DEFINE_NATIVE_METHOD(Node, remove) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Message *msg = Message::dynamicCast(message->firstInput()->value()->value());
    if(!msg) LIU_THROW(ArgumentException, "left-hand side is not a message");
    if(msg->name() == "[]") {
        msg = msg->fork();
        msg->setName("[]>>");
        return msg->run(this);
    }
    bool wasFound = true;
    removeChild(msg->name(), msg->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return this;
}

Node *Node::findChild(const QString &name, bool searchInParents, Node **parentPtr, bool autoFork, bool *isDirectPtr) const {
    Node *node = findChildInSelfOrOrigins(name, autoFork, isDirectPtr);
    if(searchInParents) {
        if(node) {
            if(parentPtr) *parentPtr = constCast(this);
        } else if(_parents)
            foreach(Node *parent, _parents->keys()) {
                if(parent != this) { // for Node::root which is child of itself
                    node = parent->findChild(name, searchInParents, parentPtr, autoFork, isDirectPtr);
                    if(node) break;
                }
            }
    }
    return node;
}

Node *Node::findChildInSelfOrOrigins(const QString &name, bool autoFork, bool *isDirectPtr) const {
    bool isRemoved;
    Node *node = hasDirectChild(name, &isRemoved);
    bool isDirect = node || isRemoved;
    if(!isDirect) {
        if(origin() != this) node = origin()->findChildInSelfOrOrigins(name);
        if(!node && _extensions) {
            foreach(Node *extension, *_extensions) {
                node = extension->findChildInSelfOrOrigins(name);
                if(node) break;
            }
        }
        if(node && autoFork) {
            node = node->fork();
            node->setIsVirtual(true);
            constCast(this)->_setChild(name, node);
        }
    }
    if(isDirectPtr) *isDirectPtr = isDirect;
    return node;
}

LIU_DEFINE_NATIVE_METHOD(Node, has) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(1);
    Message *msg = Message::dynamicCast(message->firstInput()->value()->value());
    if(!msg) LIU_THROW(ArgumentException, "argument is not message");
    Node *node = findChild(msg->name());
    if(!node) Primitive::skip(LIU_BOOLEAN(false));
    return node;
}

void Node::_addParent(Node *parent) const {
    HugeUnsignedInteger count = 0;
    if(_parents)
        count = _parents->value(parent);
    else
        _parents = new QHash<Node *, HugeUnsignedInteger>;
    _parents->insert(parent, count + 1);
}

void Node::_removeParent(Node *parent) const {
    if(!_parents) LIU_THROW(NotFoundException, "parent not found");
    HugeUnsignedInteger count = _parents->value(parent) - 1;
    if(count > 0)
        _parents->insert(parent, count);
    else if(count == 0)
        _parents->remove(parent);
    else
        LIU_THROW(NotFoundException, "parent not found");
}

QHash<QString, Node *> Node::children() const {
    QHash<QString, Node *> children;
    if(_children) {
        QHashIterator<QString, Node *> i(*_children);
        while(i.hasNext()) if(i.next().value()) children.insert(i.key(), i.value());
    }
    return children;
}

QList<Node *> Node::parents() const {
    QList<Node *> parents;
    if(_parents) foreach(Node *parent, _parents->keys()) parents.append(parent);
    return parents;
}

Node *Node::parent() const {
    if(!hasOneParent()) LIU_THROW_RUNTIME_EXCEPTION("zero or more than one parent found");
    return _parents->keys().first();
}

bool Node::hasOneParent() const {
    return _parents && _parents->size() == 1;
}

LIU_DEFINE_NATIVE_METHOD(Node, parent) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return message->isQuestioned() ? LIU_BOOLEAN(hasOneParent()) : parent();
}

Node *Node::findParentOriginatingFrom(Node *orig) const {
    if(_parents) {
        orig = orig->real();
        Node *node;
        foreach(Node *parent, _parents->keys()) {
            if(parent->isOriginatingFrom(orig)) return parent;
            if(parent != this) { // for Node::root which is child of itself
                if((node = parent->findParentOriginatingFrom(orig))) return node;
            }
        }
    }
    return NULL;
}

Node *Node::receive(Primitive *primitive) {
    return primitive->run(this);
}

LIU_DEFINE_NATIVE_METHOD(Node, or) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(toBool() || message->runFirstInput()->toBool());
}

LIU_DEFINE_NATIVE_METHOD(Node, and) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(toBool() && message->runFirstInput()->toBool());
}

LIU_DEFINE_NATIVE_METHOD(Node, not) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_BOOLEAN(!toBool());
}

LIU_DEFINE_NATIVE_METHOD(Node, or_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *lhs = message->runFirstInput();
    if(!lhs->toBool())
        return LIU_MESSAGE("=", message->firstInput(), message->secondInput())->run();
    else
        return lhs;
}

LIU_DEFINE_NATIVE_METHOD(Node, and_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *lhs = message->runFirstInput();
    if(lhs->toBool())
        return LIU_MESSAGE("=", message->firstInput(), message->secondInput())->run();
    else
        return lhs;
}

LIU_DEFINE_NATIVE_METHOD(Node, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(isEqualTo(message->runFirstInput()));
}

LIU_DEFINE_NATIVE_METHOD(Node, different_from) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(!Boolean::cast(LIU_MESSAGE("==", message->inputs(false))->run(this))->value());
}

LIU_DEFINE_NATIVE_METHOD(Node, throw) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isQuestioned()) throw *this;
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->next();
    if(!nextPrimitive)
        LIU_THROW(InterpreterException, "missing code after 'throw?' method");
    bool result = false;
    try {
        nextPrimitive->run();
    } catch(const Node &node) {
        if(node.isOriginatingFrom(this))
            result = true;
        else
            throw;
    }
    Primitive::skip(LIU_BOOLEAN(result));
}

Node *Node::assert(bool isAssertTrue) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->next();
    if(!nextPrimitive)
        LIU_THROW(InterpreterException, QString("missing code after '%1' statement").arg(message->name()));
    Node *result = nextPrimitive->run();
    if(result->toBool() != isAssertTrue) LIU_THROW(AssertionException, "assertion failed");
    passedAssertionCount()++;
    Primitive::skip(result);
}

LIU_DEFINE_NATIVE_METHOD(Node, inspect) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    inspect();
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Node, dump) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    dump();
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Node, memory_address) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_NUMBER(memoryAddress());
}

QString Node::toString(bool debug, short level) const {
    Q_UNUSED(debug);
    Q_UNUSED(level);
    return QString("%1:%2: [%3]").arg(nodeName()).arg(hexMemoryAddress(), QStringList(children().keys()).join(", "));
}

LIU_END
