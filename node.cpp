#include <QtCore/QStringList>
#include <QtCore/QSet>

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
#include "node/object/language/childcollection.h"
#include "node/object/language/parentset.h"

LIU_BEGIN

const bool Node::isInitialized = Node::root();

Node::Node(const Node &other) : _origin(other._origin), _extensions(NULL), // copy constructor
    _children(NULL), _parents(NULL), _isDefined(other._isDefined), _isUndefined(other._isUndefined),
    _isVirtual(other._isVirtual), _isAutoRunnable(other._isAutoRunnable) {
    if(other._extensions) _extensions = new QList<Node *>(*other._extensions);
    if(other._children) {
        QScopedPointer<NamedChildDictionary::Iterator> i(NamedChildDictionary::Iterator::make(&other));
        while(i->hasNext()) {
            NodeQPair child = i->next();
            addOrSetChild(child.first->toString(), child.second);
        }
    }
}

void Node::initFork() {
    if(hasOrigin()) setIsAutoRunnable(origin()->isAutoRunnable());
}

Node *Node::init() { return this; }

Node *Node::initCopy(const Node *other) {
    setIsAutoRunnable(other->isAutoRunnable());
    if(other->_extensions) _extensions = new QList<Node *>(*other->_extensions);
    if(other->_children) {
        QHashIterator<QString, Node *> i(*other->_children);
        while(i.hasNext()) { i.next(); addOrSetChild(i.key(), i.value()); }
    }
    return this;
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
        _root->setNodeOwner(_root);
        _root->declare("Node");
    }
    return _root;
}

void Node::initRoot() {
    pushContext(this);

    LIU_ADD_NATIVE_METHOD(Node, self);

    LIU_ADD_PROPERTY(Node, origin);
    LIU_ADD_READ_ONLY_PROPERTY(Node, extensions)

    LIU_ADD_NATIVE_METHOD(Node, is);

    LIU_ADD_NATIVE_METHOD(Node, defined);
    LIU_ADD_NATIVE_METHOD(Node, undefined);
    LIU_ADD_NATIVE_METHOD(Node, define);
    LIU_ADD_NATIVE_METHOD(Node, undefine);

    LIU_ADD_NATIVE_METHOD(Node, virtual);
    LIU_ADD_NATIVE_METHOD(Node, real);

    LIU_ADD_NATIVE_METHOD(Node, copy);

    LIU_ADD_NATIVE_METHOD(Node, fork);
    LIU_ADD_NATIVE_METHOD(Node, init);

    LIU_ADD_NATIVE_METHOD(Node, define_or_assign, :=);
    LIU_ADD_NATIVE_METHOD(Node, assign, =);
    LIU_ADD_NATIVE_METHOD(Node, remove, >>);

    LIU_ADD_NATIVE_METHOD(Node, has);

    LIU_ADD_NATIVE_METHOD(Node, children);
    LIU_ADD_NATIVE_METHOD(Node, parents);
    LIU_ADD_NATIVE_METHOD(Node, parent);

    LIU_ADD_NATIVE_METHOD(Node, or, ||);
    LIU_ADD_NATIVE_METHOD(Node, and, &&);
    LIU_ADD_NATIVE_METHOD(Node, not, !);

    LIU_ADD_NATIVE_METHOD(Node, or_assign, ||=);
    LIU_ADD_NATIVE_METHOD(Node, and_assign, &&=);

    LIU_ADD_NATIVE_METHOD(Node, same_as, ===);

    LIU_ADD_NATIVE_METHOD(Node, equal_to, ==);
    LIU_ADD_NATIVE_METHOD(Node, different_from, !=);

    LIU_ADD_NATIVE_METHOD(Node, throw);

    LIU_ADD_NATIVE_METHOD(Node, assert_true, ?:);
    LIU_ADD_NATIVE_METHOD(Node, assert_false, !:);

    LIU_ADD_NATIVE_METHOD(Node, inspect);
    LIU_ADD_NATIVE_METHOD(Node, dump);

    LIU_ADD_NATIVE_METHOD(Node, memory_address);
    LIU_ADD_NATIVE_METHOD(Node, hash);
}

const QString Node::nodeName() const {
    return child(".name")->toString();
}

void Node::setNodeName(const QString &name) {
    addOrSetChild(".name", (new Text(Text::root()))->init(&name));
}

Node *Node::nodeOwner() const {
    return child(".owner");
}

void Node::setNodeOwner(Node *owner) {
    addOrSetChild(".owner", owner);
}

const QString Node::nodePath() const {
    QString path;
    const Node *cur = this;
    const Node *par;
    while((par = cur->nodeOwner()) != cur) {
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
    Primitive *nextPrimitive = primitive->hasNext();
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

bool Node::isOriginatingFrom(Node *orig) const { // TODO: Optimize and secure with a alreadySeen cache
    orig = orig->real();
    const Node *node = real();
    while(node != orig) {
        if(node->_extensions) {
            foreach(Node *extension, *node->_extensions)
                if(extension->isOriginatingFrom(orig)) return true;
        }
        if(node->origin() == node) // Node::root reached?
            return false;
        node = node->origin();
    }
    return true;
}

LIU_DEFINE_NATIVE_METHOD(Node, is) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(1);
    return Boolean::make(isOriginatingFrom(message->runFirstInput()));
}

bool Node::isDefined(QSet<const Node *> *alreadySeen) const {
    if(_isDefined) return true;
    if(_isUndefined) return false;
    bool firstRecursion = alreadySeen == NULL;
    if(firstRecursion) alreadySeen = new QSet<const Node *>;
    bool result = false;
    if(!alreadySeen->contains(this)) {
        alreadySeen->insert(this);
        QScopedPointer<ChildCollection::Iterator> i(ChildCollection::Iterator::make(this));
        while(!result && i->hasNext()) result = i->next().second->isDefined(alreadySeen);
        if(!result && origin() != this) result = origin()->isDefined(alreadySeen);
    }
    if(firstRecursion) delete alreadySeen; // FIXME: potential memory leak
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Node, defined) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return Boolean::make(isDefined());
}

LIU_DEFINE_NATIVE_METHOD(Node, undefined) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return Boolean::make(!isDefined());
}

LIU_DEFINE_NATIVE_METHOD(Node, define) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_EXCLAMATION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    setIsDefined(true);
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Node, undefine) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_EXCLAMATION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    setIsDefined(false);
    return this;
}

Node *Node::virtualOrReal(bool virtualMode) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return Boolean::make(isVirtual() == virtualMode);
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
    if(Property *property = Property::dynamicCast(findChild(msg->name()))) {
        Message *setMessage = LIU_MESSAGE("set", value);
        setMessage->setModifiers(msg->modifiers());
        setMessage->run(property);
    } else
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
//    if(msg->name() == "[]") {
//        msg = msg->fork();
//        msg->setName("[]>>");
//        return msg->run(this);
//    }
    bool wasFound = true;
    removeChild(msg->name(), msg->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return this;
}

Node *Node::findChild(const QString &name, bool searchInParents, Node **parentPtr, bool autoFork, bool *isDirectPtr) const {
    QScopedPointer< QSet<const Node *> > alreadySeen(new QSet<const Node *>);
    return findChildInSelfOriginsOrParents(name, searchInParents, parentPtr, autoFork, isDirectPtr, alreadySeen.data());
}

Node *Node::findChildInSelfOriginsOrParents(const QString &name, bool searchInParents, Node **parentPtr,
                                            bool autoFork, bool *isDirectPtr, QSet<const Node *> *alreadySeen) const {
    Node *node = findChildInSelfOrOrigins(name, autoFork, isDirectPtr, alreadySeen);
    if(searchInParents) {
        if(node) {
            if(parentPtr) *parentPtr = constCast(this);
        } else if(_parents)
            foreach(Node *parent, _parents->keys()) {
                if(parent != this && !alreadySeen->contains(parent)) { // for Node::root which is child of itself
                    node = parent->findChildInSelfOriginsOrParents(name, searchInParents, parentPtr,
                                                                   autoFork, isDirectPtr, alreadySeen);
                    if(node) break;
                }
            }
    }
    return node;
}

Node *Node::findChildInSelfOrOrigins(const QString &name, bool autoFork,
                                     bool *isDirectPtr, QSet<const Node *> *alreadySeen) const {
    if(alreadySeen->contains(this)) return NULL;
    alreadySeen->insert(this);
    bool isRemoved;
    Node *node = hasDirectChild(name, &isRemoved);
    bool isDirect = node || isRemoved;
    if(!isDirect) {
        if(origin() != this) node = origin()->findChildInSelfOrOrigins(name, true, NULL, alreadySeen);
        if(!node && _extensions) {
            foreach(Node *extension, *_extensions) {
                node = extension->findChildInSelfOrOrigins(name, true, NULL, alreadySeen);
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
    return Boolean::make(findChild(msg->name()) != NULL);
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

LIU_DEFINE_NATIVE_METHOD(Node, children) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return NamedChildDictionary::make(this);
}

LIU_DEFINE_NATIVE_METHOD(Node, parents) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return ParentSet::make(this);
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
    return message->isQuestioned() ? Boolean::make(hasOneParent()) : parent();
}

Node *Node::findParentOriginatingFrom(Node *orig) const {
    QScopedPointer< QSet<const Node *> > alreadySeen(new QSet<const Node *>);
    return real()->_findParentOriginatingFrom(orig->real(), alreadySeen.data());
}

Node *Node::_findParentOriginatingFrom(Node *orig, QSet<const Node *> *alreadySeen) const {
    if(alreadySeen->contains(this)) return NULL;
    if(isOriginatingFrom(orig)) return constCast(this);
    alreadySeen->insert(this);
    if(_parents) {
        foreach(Node *parent, _parents->keys())
            if(Node *node = parent->_findParentOriginatingFrom(orig, alreadySeen)) return node;
    }
    if(Node *node = origin()->_findParentOriginatingFrom(orig, alreadySeen)) return node;
    return NULL;
}

Node *Node::receive(Primitive *primitive) {
//    primitive->value()->inspect();
//    inspect();
    return primitive->run(this);
}

LIU_DEFINE_NATIVE_METHOD(Node, or) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Boolean::make(toBool() || message->runFirstInput()->toBool());
}

LIU_DEFINE_NATIVE_METHOD(Node, and) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Boolean::make(toBool() && message->runFirstInput()->toBool());
}

LIU_DEFINE_NATIVE_METHOD(Node, not) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return Boolean::make(!toBool());
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

LIU_DEFINE_NATIVE_METHOD(Node, same_as) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Boolean::make(isSameAs(message->runFirstInput()));
}

LIU_DEFINE_NATIVE_METHOD(Node, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Boolean::make(isEqualTo(message->runFirstInput()));
}

LIU_DEFINE_NATIVE_METHOD(Node, different_from) {
    LIU_FIND_LAST_MESSAGE;
    return Boolean::make(!Boolean::cast(LIU_MESSAGE("==", message->inputs(false))->run(this))->value());
}

short Node::compare(const Node *other) const {
    Q_UNUSED(other);
    LIU_ABSTRACT_CALL;
}

LIU_DEFINE_NATIVE_METHOD(Node, throw) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isQuestioned()) throw this;
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->hasNext();
    if(!nextPrimitive) LIU_THROW(InterpreterException, "missing code after 'throw?' method");
    bool result = false;
    try {
        nextPrimitive->run();
    } catch(Node *node) {
        if(node->isOriginatingFrom(this))
            result = true;
        else
            throw;
    }
    Primitive::skip(Boolean::make(result));
}

Node *Node::assert(bool isAssertTrue) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->hasNext();
    if(!nextPrimitive)
        LIU_THROW(InterpreterException, QString("missing code after '%1' statement").arg(message->name()));
    Node *result = nextPrimitive->run();
    if(result->toBool() != isAssertTrue) LIU_THROW(AssertionException, "assertion failed");
    passedAssertionCount()++;
    Primitive::skip(result);
}

void Node::inspectInternal() const { LIU_ABSTRACT_CALL; }

LIU_DEFINE_NATIVE_METHOD(Node, inspect) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isExclaimed()) inspectInternal(); else inspect();
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
    return Number::make(memoryAddress());
}

QString Node::toString(bool debug, short level) const {
    Q_UNUSED(debug);
    Q_UNUSED(level);
    QScopedPointer<NamedChildDictionary::Iterator> i(NamedChildDictionary::Iterator::make(constCast(this)));
    QString str;
    while(i->hasNext()) {
        if(!str.isEmpty()) str += ", ";
        str += i->next().first->toString();
    }
    return QString("%1:%2: [%3]").arg(nodeName()).arg(hexMemoryAddress()).arg(str);
}

LIU_DEFINE_NATIVE_METHOD(Node, hash) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return Number::make(hash());
}

LIU_END
