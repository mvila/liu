#include "node/object/list.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === List ===

LIU_DEFINE_2(List, Object, Object);

List *List::init() {
    Object::init();
    _operations = NULL;
    return this;
}

List::~List() {
    if(_operations) delete _operations;
}

void List::initRoot() {
    addExtension(Insertable::root());
}

QString List::toString(bool debug, short level) const {
    if(!debug) return Insertable::toString(debug, level);
    QString str;
    if(!_operations)
        str = "_operations is NULL";
    else {
        str = "[";
        foreach(const Operation &operation, *_operations) {
            if(str.size() != 1)
                str += ", ";
            if(operation.type == Operation::Set)
                str += "S";
            else if(operation.type == Operation::Insert)
                str += "I";
            else if (operation.type == Operation::Remove)
                str += "R";
            str += QString("[%1..%2]").arg(operation.index).arg(operation.index + operation.size - 1);
        }
        str += "]";
    }
    return str;
}

// --- Iterable ---

List::Iterator *List::iterator() const {
    return List::Iterator::make(constCast(this));
}

int List::size() const {
    int result = 0;
    if(_operations) {
        foreach(const Operation &operation, *_operations) {
            if(operation.type == Operation::Insert)
                result += operation.size;
            else if (operation.type == Operation::Remove)
                result -= operation.size;
        }
    }
    if(List *orig = List::dynamicCast(origin())) result += orig->size();
    return result;
}

// --- Collection ---

void List::append(Node *item) {
    _insert(size(), item);
}

Node *List::remove(Node *item, bool *wasFoundPtr) {
//    QString source = value();
//    QString str = item->toString();
//    int index = source.indexOf(str);
//    bool wasFound = index != -1;
//    Text *result = NULL;
//    if(wasFound) {
//        result = Text::make(source.mid(index, str.size()));
//        source.remove(index, str.size());
//        setValue(source);
//    }
//    if(wasFoundPtr)
//        *wasFoundPtr = wasFound;
//    else if(!wasFound)
//        LIU_THROW(NotFoundException, "value not found");
//    return result;
    Q_UNUSED(item);
    Q_UNUSED(wasFoundPtr);
    return NULL;
}

// --- Indexable ---

Node *List::get(Node *nodeIndex, bool *wasFoundPtr) {
    int index = nodeIndex->toDouble();
    int max = size();
    if(index < 0) index = max + index;
    bool wasFound = index >= 0 && index < max;
    Node *result = wasFound ? _get(index) : NULL;
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    return result;
}

Node *List::_get(int index) {
    if(_operations) {
        foreach(const Operation &operation, *_operations) {
            if(operation.index > index) break;
            if(operation.data && index < operation.index + operation.size)
                return operation.data->at(index - operation.index);
            else if(operation.type == Operation::Insert)
                index -= operation.size;
            else if(operation.type == Operation::Remove)
                index += operation.size;
        }
    }
    if(List *orig = List::dynamicCast(origin())) return orig->_get(index);
    LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
}

void List::set(Node *nodeIndex, Node *nodeValue, bool *wasFoundPtr) {
//    int index = nodeIndex->toDouble();
//    QString str = value();
//    int max = str.size();
//    if(index < 0) index = max + index;
//    bool wasFound = index >= 0 && index < max;
//    if(wasFound) {
//        str.remove(index, 1);
//        str.insert(index, nodeValue->toString());
//        setValue(str);
//    }
//    if(wasFoundPtr)
//        *wasFoundPtr = wasFound;
//    else if(!wasFound)
//        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    Q_UNUSED(nodeIndex);
    Q_UNUSED(nodeValue);
    Q_UNUSED(wasFoundPtr);
}

void List::append(Node *nodeIndex, Node *nodeValue, bool *okPtr) {
    int max = size();
    int index = nodeIndex ? nodeIndex->toDouble() : max;
    if(index < 0) index = max + index;
    bool ok = index == max;
    if(ok) _insert(index, nodeValue);
    if(okPtr)
        *okPtr = ok;
    else if(!ok) {
        if(index < max)
            LIU_THROW(DuplicateException, "index already exists");
        else
            LIU_THROW(IndexOutOfBoundsException, "index is invalid");
    }
}

Node *List::unset(Node *nodeIndex, bool *wasFoundPtr) {
//    int index = nodeIndex->toDouble();
//    QString str = value();
//    int max = str.size();
//    if(index < 0) index = max + index;
//    bool wasFound = index >= 0 && index < max;
//    Character *result = NULL;
//    if(wasFound) {
//        result = LIU_CHARACTER(str.at(index));
//        str.remove(index, 1);
//        setValue(str);
//    }
//    if(wasFoundPtr)
//        *wasFoundPtr = wasFound;
//    else if(!wasFound)
//        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
//    return result;
    Q_UNUSED(nodeIndex);
    Q_UNUSED(wasFoundPtr);
    return NULL;
}

List::IndexIterator *List::indexIterator() const {
    return List::IndexIterator::make(constCast(this));
}

// --- Insertable ---

void List::insert(Node *nodeIndex, Node *nodeValue, bool *okPtr) {
    int max = size();
    int index = nodeIndex ? nodeIndex->toDouble() : max;
    if(index < 0) index = max + index;
    bool ok = index >= 0 && index <= max;
    if(ok) _insert(index, nodeValue);
    if(okPtr)
        *okPtr = ok;
    else if(!ok) {
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    }
}

void List::_insert(int index, Node *item) {
    if(!_operations) _operations = new QList<Operation>;
    int size = _operations->size();
    int i;
    for (i = 0; i < size; ++i) {
        Operation &operation = (*_operations)[i];
        if(operation.index > index) break;
        if(operation.type == Operation::Insert) {
            if(index <= operation.index + operation.size) {
                operation.data->insert(index - operation.index, item);
                operation.size++;
                return;
            } else
                index -= operation.size;
        } else if(operation.type == Operation::Remove)
            index += operation.size;
    }
    QList<Node *> *data = new QList<Node *>;
    data->append(item);
    _operations->insert(i, Operation(Operation::Insert, index, 1, data));
}

// === List::Iterator ===

LIU_DEFINE_2(List::Iterator, Iterable::Iterator, List);

List::Iterator *List::Iterator::init(List **list, int *index) {
    Object::init();
    _list = NULL;
    _index = NULL;
    setList(list);
    setIndex(index);
    return this;
}

List::Iterator::~Iterator() {
    setList();
    setIndex();
}

void List::Iterator::initRoot() {
}

LIU_DEFINE_ACCESSOR(List::Iterator, List::ListPtr, list, List, NULL);
LIU_DEFINE_ACCESSOR(List::Iterator, int, index, Index, 0);

bool List::Iterator::hasNext() const {
    if(!list()) return false;
    return index() < list()->size();
}

Node *List::Iterator::peekNext() const {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return list()->_get(index());
}

void List::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    setIndex(index() + 1);
}

// === List::IndexIterator ===

LIU_DEFINE_2(List::IndexIterator, Iterable::Iterator, List);

List::IndexIterator *List::IndexIterator::init(List **list, int *index) {
    Object::init();
    _list = NULL;
    _index = NULL;
    setList(list);
    setIndex(index);
    return this;
}

List::IndexIterator::~IndexIterator() {
    setList();
    setIndex();
}

void List::IndexIterator::initRoot() {
}

LIU_DEFINE_ACCESSOR(List::IndexIterator, List::ListPtr, list, List, NULL);
LIU_DEFINE_ACCESSOR(List::IndexIterator, int, index, Index, 0);

bool List::IndexIterator::hasNext() const {
    if(!list()) return false;
    return index() < list()->size();
}

Number *List::IndexIterator::peekNext() const {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "IndexIterator is out of bounds");
    return LIU_NUMBER(index());
}

void List::IndexIterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "IndexIterator is out of bounds");
    setIndex(index() + 1);
}

LIU_END
