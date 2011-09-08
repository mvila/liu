#ifndef LIU_LIST_H
#define LIU_LIST_H

#include "node/object.h"

LIU_BEGIN

#define LIU_CHECK_VALUE(VALUE) \
if(!(VALUE)) LIU_THROW(NullPointerException, "value is NULL")

// === GenericAbstractList ===

template<class T>
class GenericAbstractList : public Object {
public:
    explicit GenericAbstractList(Node *origin) : Object(origin), _areDuplicateValuesNotAllowed(false) {}

    virtual void initFork() {
        Object::initFork();
        GenericAbstractList *orig = static_cast<GenericAbstractList *>(origin());
        setAreDuplicateValuesNotAllowed(orig->areDuplicateValuesNotAllowed());
    }

    bool areDuplicateValuesNotAllowed() const { return _areDuplicateValuesNotAllowed; }
    void setAreDuplicateValuesNotAllowed(bool value) { _areDuplicateValuesNotAllowed = value; }

    T get(int i, bool *wasFoundPtr = NULL) const {
        bool wasFound = hasIndex(i);
        if(wasFoundPtr)
            *wasFoundPtr = wasFound;
        else if(!wasFound)
            LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
        return wasFound ? doGet(i) : NULL;
    }

    virtual T doGet(int i) const {
        Q_UNUSED(i);
        LIU_ABSTRACT_CALL;
    }

    T first() const { return get(0); }
    T second() const { return get(1); }
    T third() const { return get(2); }
    T fourth() const { return get(3); }
    T fifth() const { return get(4); }
    T last() const { return get(size()-1); }

    int get(T *value, bool *wasFoundPtr = NULL) const {
        LIU_CHECK_VALUE(*value);
        bool wasFound = false;
        int i;
        T val;
        for(i = 0; i < size(); ++i) {
            val = get(i);
            if(val->isEqualTo(*value)) {
                wasFound = true;
                *value = val;
                break;
            }
        }
        if(wasFoundPtr)
            *wasFoundPtr = wasFound;
        else if(!wasFound)
            LIU_THROW(NotFoundException, "value not found");
        return i;
    }

    T set(int i, const T &value) {
        checkIndex(i);
        LIU_CHECK_VALUE(value);
        if(!doGet(i)->isEqualTo(value)) {
            checkIfValueIsAllowed(value);
            doSet(i, value);
            hasChanged();
        }
        return value;
    }

    virtual void doSet(int i, const T &value) {
        Q_UNUSED(i);
        Q_UNUSED(value);
        LIU_ABSTRACT_CALL;
    }

    T insert(int i, const T &value) {
        checkIndex(i, true);
        LIU_CHECK_VALUE(value);
        checkIfValueIsAllowed(value);
        doInsert(i, value);
        hasChanged();
        return value;
    }

    virtual void doInsert(int i, const T &value) {
        Q_UNUSED(i);
        Q_UNUSED(value);
        LIU_ABSTRACT_CALL;
    }

    const GenericAbstractList *insert(int i, const GenericAbstractList *otherList) {
        checkIndex(i, true);
        if(!otherList) LIU_THROW(NullPointerException, "List pointer is NULL");
        for(int j = 0; j < otherList->size(); j++) {
            doInsert(i + j, otherList->get(j));
        }
        hasChanged();
        return otherList;
    }

    T append(const T &value) { return insert(size(), value); }
    const GenericAbstractList *append(const GenericAbstractList *otherList) { return insert(size(), otherList); }
    T prepend(const T &value) { return insert(0, value); }
    const GenericAbstractList *prepend(const GenericAbstractList *otherList) { return insert(0, otherList); }

    void remove(int i) {
        checkIndex(i);
        doRemove(i);
        hasChanged();
    }

    virtual void doRemove(int i) {
        Q_UNUSED(i);
        LIU_ABSTRACT_CALL;
    }

    void clear() {
        if(size() > 0) {
            doClear();
            hasChanged();
        }
    }

    virtual void doClear() {
        LIU_ABSTRACT_CALL;
    }

    bool hasIndex(int i) const { return i >= 0 && i < size(); }

    bool hasValue(T value) const {
        bool wasFound;
        get(&value, &wasFound);
        return wasFound;
    }

    void checkIfValueIsAllowed(const T &value) const {
        if(areDuplicateValuesNotAllowed() && hasValue(value))
            LIU_THROW(DuplicateException, "cannot add something which is already there");
    }

    virtual int size() const {
        LIU_ABSTRACT_CALL;
    }
    bool isEmpty() const { return size() == 0; }
    bool isNotEmpty() const { return size() > 0; }

    const QString join(const QString &separator = "", const QString &prefix = "",
                       const QString &suffix = "", bool debug = false, short level = 0) const {
        QString str;
        for(int i = 0; i < size(); ++i) {
            if(i > 0) str += separator;
            str += prefix + get(i)->toString(debug, level) + suffix;
        }
        return str;
    }

    void checkIndex(int i, bool insertMode = false) const {
        int max = size();
        if(!insertMode) max--;
        if(i < 0) LIU_THROW(IndexOutOfBoundsException, "index is less than zero");
        if(i > max) {
            if(insertMode)
                LIU_THROW(IndexOutOfBoundsException, "index is greater than size");
            else
                LIU_THROW(IndexOutOfBoundsException, "index is greater than size-1");
        }
    }

    virtual void hasChanged() {}

    virtual QString toString(bool debug = false, short level = 0) const {
        return "[" + join(", ", "", "", debug, level) + "]";
    }
private:
    bool _areDuplicateValuesNotAllowed;
};

// === AbstractList ===

#define LIU_ABSTRACT_LIST(ARGS...) new AbstractList(context()->child("Object", "AbstractList"), ##ARGS)

class AbstractList : public GenericAbstractList<Node *> {
    LIU_DECLARE(AbstractList, Object, Object);
public:
    explicit AbstractList(Node *origin) : GenericAbstractList<Node *>(origin) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(AbstractList);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(AbstractList);

    LIU_DECLARE_NATIVE_METHOD(get);
    LIU_DECLARE_NATIVE_METHOD(set);
    LIU_DECLARE_NATIVE_METHOD(append_or_set);

    LIU_DECLARE_NATIVE_METHOD(append);
    LIU_DECLARE_NATIVE_METHOD(remove);

    LIU_DECLARE_NATIVE_METHOD(size);
    LIU_DECLARE_NATIVE_METHOD(empty);
};

// === GenericList ===

template<class T>
class GenericList : public GenericAbstractList<T> {
public:
    using GenericAbstractList<T>::origin;
    using GenericAbstractList<T>::removeAnonymousChild;
    using GenericAbstractList<T>::checkIndex;
    using GenericAbstractList<T>::hasChanged;

    explicit GenericList(Node *origin, bool isBunch = false) :
        GenericAbstractList<T>(origin), _list(NULL), _isBunch(isBunch) {}

    GenericList(Node *origin, const T &value, bool isBunch = false) :
        GenericAbstractList<T>(origin), _list(NULL), _isBunch(isBunch) { append(value); }

    GenericList(Node *origin, const T &value1, const T &value2, bool isBunch = false) :
        GenericAbstractList<T>(origin), _list(NULL), _isBunch(isBunch) { append(value1); append(value2); }

    GenericList(Node *origin, const T &value1, const T &value2, const T &value3, bool isBunch = false) :
        GenericAbstractList<T>(origin), _list(NULL), _isBunch(isBunch) { append(value1); append(value2); append(value3); }

    GenericList(Node *origin, const QList<T> &other, bool isBunch = false) :
        GenericAbstractList<T>(origin), _list(NULL), _isBunch(isBunch) {
        if(!other.isEmpty()) {
            foreach(T node, other) doInsert(size(), node);
            hasChanged();
        }
    }

    GenericList(const GenericList &other) : GenericAbstractList<T>(other), _list(NULL), _isBunch(other._isBunch) {
        if(other.isNotEmpty()) {
            foreach(T node, *other._list) doInsert(size(), node);
            hasChanged();
        }
    }

    virtual ~GenericList() {
        if(_list) {
            foreach(T node, *_list) removeAnonymousChild(node);
            delete _list;
        }
    }

    virtual void initFork() {
        GenericAbstractList<T>::initFork();
        GenericList *orig = static_cast<GenericList *>(origin());
        if(orig->isNotEmpty()) {
            foreach(T node, *orig->_list) doInsert(size(), node->fork());
            hasChanged();
        }
    }

    virtual T doGet(int i) const {
        return _list->at(i);
    }

    virtual void doSet(int i, const T &value) {
        removeAnonymousChild(_list->at(i));
        _list->replace(i, value);
        addAnonymousChild(value);
    }

    virtual void doInsert(int i, const T &value) {
        if(!_list) { _list = new QList<T>; }
        _list->insert(i, value);
        addAnonymousChild(value);
    }

    virtual void doRemove(int i) {
        removeAnonymousChild(_list->at(i));
        _list->removeAt(i);
    }

    virtual void doClear() {
        if(_list) {
            foreach(Node *node, *_list) removeAnonymousChild(node);
            _list->clear();
        }
    }

    virtual int size() const { return _list ? _list->size() : 0; }
private:
    QList<T> *_list;
    bool _isBunch;
public:
    class Iterator {
    public:
        Iterator(const GenericList *list) : _iterator(list->_list ? new QListIterator<T>(*list->_list) : NULL) {}
        ~Iterator() { delete _iterator; }

        bool hasNext() const { return _iterator && _iterator->hasNext(); }
        const T next() { return hasNext() ? _iterator->next() : NULL; }
    private:
        QListIterator<T> *_iterator;
    };
};

// === List ===

#define LIU_LIST(ARGS...) new List(context()->child("Object", "List"), ##ARGS)

class List : public GenericList<Node *> {
    LIU_DECLARE(List, AbstractList, Object);
public:
    explicit List(Node *origin) : GenericList<Node *>(origin) {}

    List(Node *origin, const QList<Node *> &other) : GenericList<Node *>(origin, other) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(List);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(List);

    LIU_DECLARE_NATIVE_METHOD(init);
    LIU_DECLARE_NATIVE_METHOD(make);
};

// === GenericVirtualList ===

template<class T>
class GenericVirtualList : public GenericAbstractList<T> {
public:
    using GenericAbstractList<T>::checkIndex;
    using GenericAbstractList<T>::hasChanged;

    explicit GenericVirtualList(Node *origin, QList<T> **source = NULL) : GenericAbstractList<T>(origin), _source(source) {}

    GenericVirtualList(const GenericVirtualList &other) : GenericAbstractList<T>(other), _source(other._source) {}

    QList<T> **source() const { return _source; }
    void setSource(QList<T> **source) { _source = source; }

    virtual T doGet(int i) const {
        return (*_source)->at(i);
    }

    virtual void doSet(int i, const T &value) {
        (*_source)->replace(i, value);
    }

    virtual void doInsert(int i, const T &value) {
        if(!*_source) { *_source = new QList<T>; }
        (*_source)->insert(i, value);
    }

    virtual void doRemove(int i) {
        (*_source)->removeAt(i);
    }

    virtual void doClear() {
        if(*_source) (*_source)->clear();
    }

    virtual int size() const { return *_source ? (*_source)->size() : 0; }
private:
    QList<T> **_source;
};

// === VirtualList ===

#define LIU_VIRTUAL_LIST(ARGS...) new VirtualList(context()->child("Object", "VirtualList"), ##ARGS)

class VirtualList : public GenericVirtualList<Node *> {
    LIU_DECLARE(VirtualList, AbstractList, Object);
public:
    explicit VirtualList(Node *origin, QList<Node *> **source = NULL) : GenericVirtualList<Node *>(origin, source) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(VirtualList);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(VirtualList, source());
};

LIU_END

#endif // LIU_LIST_H
