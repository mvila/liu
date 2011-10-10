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
    delete _operations;
}

void List::initRoot() {
    addExtension(Insertable::root());

    LIU_ADD_NATIVE_METHOD(List, make);
}

LIU_DEFINE_NATIVE_METHOD(List, make) {
    LIU_FIND_LAST_MESSAGE;
    List *list = fork();
    for(int i = 0; i < message->numInputs(); ++i)
        list->append(message->runInput(i));
    return list;
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
    Node *result = NULL;
    bool wasFound;
    Node *number = index(item, &wasFound);
    if(wasFound) result = unset(number);
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(NotFoundException, "value not found");
    return result;
}

void List::clear() {
    if(_operations) _operations->clear();
    int s = size();
    if(s > 0) {
        if(!_operations) _operations = new QList<Operation>;
        _operations->append(Operation(Operation::Remove, 0, s));
    }
}

// --- Indexable ---

Node *List::get(Node *index, bool *wasFoundPtr) {
    int i = index->toDouble();
    int max = size();
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i < max;
    Node *result = wasFound ? _get(i) : NULL;
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
            if((operation.type == Operation::Set || operation.type == Operation::Insert)
                    && index < operation.index + operation.size)
                return operation.data.at(index - operation.index);
            else if(operation.type == Operation::Insert)
                index -= operation.size;
            else if(operation.type == Operation::Remove)
                index += operation.size;
        }
    }
    if(List *orig = List::dynamicCast(origin())) return orig->_get(index);
    LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
}

void List::set(Node *index, Node *item, bool *wasFoundPtr) {
    int max = size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i < max;
    if(wasFound) _set(i, item);
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound) {
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    }
}

void List::_set(int index, Node *item) {
    if(!_operations) _operations = new QList<Operation>;
    int i;
    int size = _operations->size();
    for(i = 0; i < size; ++i) {
        Operation &operation = (*_operations)[i];
        if(operation.type == Operation::Set && operation.index - 1 <= index
                && index <= operation.index + operation.size) {
            if(index == operation.index - 1) { // S[1..2] + S[0] -> S[0..2]
                operation.data.prepend(item);
                operation.index--;
                operation.size++;
            } else if(index == operation.index + operation.size) { // S[1..2] + S[3] -> S[1..3]
                operation.data.append(item);
                operation.size++;
            } else // S[1..2] + S[1] -> S[1..2]
                operation.data.replace(index - operation.index, item);
            return;
        }
        if(operation.index > index) break;
        if(operation.type == Operation::Insert) {
            if(index < operation.index + operation.size) { // I[1..2] + S[1] -> I[1..2]
                operation.data.replace(index - operation.index, item);
                return;
            } else
                index -= operation.size;
        } else if(operation.type == Operation::Remove)
            index += operation.size;
    }
    QList<Node *> data;
    data.append(item);
    _operations->insert(i, Operation(Operation::Set, index, 1, data));
}

void List::append(Node *index, Node *value, bool *okPtr) {
    int max = size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool ok = i == max;
    if(ok) _insert(i, value);
    if(okPtr)
        *okPtr = ok;
    else if(!ok) {
        if(i < max)
            LIU_THROW(DuplicateException, "index already exists");
        else
            LIU_THROW(IndexOutOfBoundsException, "index is invalid");
    }
}

Node *List::unset(Node *index, bool *wasFoundPtr) {
    int i = index->toDouble();
    int max = size();
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i < max;
    Node *result = NULL;
    if(wasFound) {
        result = _get(i);
        _unset(i);
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound) {
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    }
    return result;
}

void List::_unset(int index) {
    if(!_operations) _operations = new QList<Operation>;
    int i;
    int size = _operations->size();
    for(i = 0; i < size; ++i) {
        Operation &operation = (*_operations)[i];
        if(operation.type == Operation::Remove &&
                (index == operation.index - 1 || index == operation.index)) {
            if(index == operation.index - 1) operation.index--; // R[1..2] + R[0] -> R[0..2]
            operation.size++; // R[1..2] + R[1] -> R[1..3]
            return;
        }
        if(operation.index > index) break;
        if(operation.type == Operation::Set) {
            if(index < operation.index + operation.size) { // S[1..3] + R[2] -> S[1..1] + R[2..2] + S[3..3]
                int s = index - operation.index;
                QList<Node *> data;
                for(int j = 0; j < s; ++j) data.append(operation.data.takeAt(0));
                int indexCopy = operation.index;
                operation.size -= s;
                if(operation.size > 1) {
                    operation.data.removeFirst();
                    operation.size--;
                    operation.index = index + 1;
                } else
                    _operations->removeAt(i);
                if(s > 0) _operations->insert(i++, Operation(Operation::Set, indexCopy, s, data));
                break;
            }
        } else if(operation.type == Operation::Insert) { // I[1..3] + R[2] -> I[1..2]
            if(index < operation.index + operation.size) {
                if(operation.size > 1) {
                    operation.data.removeAt(index - operation.index);
                    operation.size--;
                } else
                    _operations->removeAt(i);
                return;
            } else
                index -= operation.size;
        } else if(operation.type == Operation::Remove)
            index += operation.size;
    }
    _operations->insert(i, Operation(Operation::Remove, index, 1));
}

List::IndexIterator *List::indexIterator() const {
    return List::IndexIterator::make(constCast(this));
}

// --- Insertable ---

void List::insert(Node *index, Node *item, bool *okPtr) {
    int max = size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool ok = i >= 0 && i <= max;
    if(ok) _insert(i, item);
    if(okPtr)
        *okPtr = ok;
    else if(!ok) {
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    }
}

void List::_insert(int index, Node *item) {
    if(!_operations) _operations = new QList<Operation>;
    int i;
    int size = _operations->size();
    for(i = 0; i < size; ++i) {
        Operation &operation = (*_operations)[i];
        if(operation.index > index) break;
        if(operation.type == Operation::Set) {
            if(index < operation.index + operation.size) {
                if(index > operation.index) { // S[1..3] + I[2] -> S[1] + I[2] + S[2..3]
                    int s = index - operation.index;
                    QList<Node *> data;
                    for(int j = 0; j < s; ++j) data.append(operation.data.takeAt(0));
                    operation.size -= s;
                    int indexCopy = operation.index;
                    operation.index = index;
                    _operations->insert(i++, Operation(Operation::Set, indexCopy, s, data));
                }
                break;
            }
        } else if(operation.type == Operation::Insert) {
            if(index <= operation.index + operation.size) { // I[1..2] + I[1] -> I[1..3]
                operation.data.insert(index - operation.index, item);
                operation.size++;
                return;
            } else
                index -= operation.size;
        } else if(operation.type == Operation::Remove) {
            if(index == operation.index) { // R[1..2] + I[1] -> S[1] + R[2]
                if(operation.size > 1) {
                    operation.index++;
                    operation.size--;
                } else
                    _operations->removeAt(i);
                QList<Node *> data;
                data.append(item);
                _operations->insert(i, Operation(Operation::Set, index, 1, data));
                return;
            } else
                index += operation.size;
        }
    }
    QList<Node *> data;
    data.append(item);
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
