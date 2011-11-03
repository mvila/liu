#include "node/object/list.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === List ===

LIU_DEFINE_2(List, Object, Object);

List *List::init() {
    Insertable::init();
    return this;
}

List *List::initCopy(const List *other) {
    Insertable::initCopy(other);
    if(other->_operations) _operations = new QList<Operation>(*other->_operations);
    return this;
}

List::~List() {
    delete _operations;
}

void List::initRoot() {
    addExtension(Insertable::root());

    LIU_ADD_NATIVE_METHOD(List, make);

    LIU_ADD_NATIVE_METHOD(List, sort);
}

LIU_DEFINE_NATIVE_METHOD(List, make) {
    LIU_FIND_LAST_MESSAGE;
    List *list = List::make();
    for(int i = 0; i < message->numInputs(); ++i)
        list->append(message->runInput(i));
    return list;
}

void List::inspectInternal() const {
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
    P(str.toUtf8());
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
        _operations->append(Operation(this, Operation::Remove, 0, s));
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
    int i = index;
    if(_operations) {
        foreach(const Operation &operation, *_operations) {
            if(operation.index > i) break;
            if((operation.type == Operation::Set || operation.type == Operation::Insert)
                    && i < operation.index + operation.size)
                return operation.oldData.at(i - operation.index);
            else if(operation.type == Operation::Insert)
                i -= operation.size;
            else if(operation.type == Operation::Remove)
                i += operation.size;
        }
    }
    if(List *orig = List::dynamicCast(origin())) {
        Node *result = orig->_get(i);
        result = result->fork();
        result->setIsVirtual(true);
        constCast(this)->_set(index, result);
        return result;
    }
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
                operation.oldData.prepend(item);
                operation.index--;
                operation.size++;
            } else if(index == operation.index + operation.size) { // S[1..2] + S[3] -> S[1..3]
                operation.oldData.append(item);
                operation.size++;
                if(i + 1 < size) {
                    Operation &nextOperation = (*_operations)[i + 1];
                    if(nextOperation.type == Operation::Set && nextOperation.index == index) {
                        if(nextOperation.size > 1) {
                            nextOperation.oldData.removeAt(0);
                            nextOperation.index++;
                            nextOperation.size--;
                        } else
                            _operations->removeAt(i + 1);
                    }
                }
            } else // S[1..2] + S[1] -> S[1..2]
                operation.oldData.replace(index - operation.index, item);
            return;
        }
        if(operation.index > index) break;
        if(operation.type == Operation::Insert) {
            if(index < operation.index + operation.size) { // I[1..2] + S[1] -> I[1..2]
                operation.oldData.replace(index - operation.index, item);
                return;
            } else
                index -= operation.size;
        } else if(operation.type == Operation::Remove)
            index += operation.size;
    }
    QList<Node *> data;
    data.append(item);
    _operations->insert(i, Operation(this, Operation::Set, index, 1, data));
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
                for(int j = 0; j < s; ++j) data.append(operation.oldData.takeAt(0));
                int indexCopy = operation.index;
                operation.size -= s;
                if(operation.size > 1) {
                    operation.oldData.removeFirst();
                    operation.size--;
                    operation.index = index + 1;
                } else
                    _operations->removeAt(i);
                if(s > 0) _operations->insert(i++, Operation(this, Operation::Set, indexCopy, s, data));
                break;
            }
        } else if(operation.type == Operation::Insert) { // I[1..3] + R[2] -> I[1..2]
            if(index < operation.index + operation.size) {
                if(operation.size > 1) {
                    operation.oldData.removeAt(index - operation.index);
                    operation.size--;
                } else
                    _operations->removeAt(i);
                return;
            } else
                index -= operation.size;
        } else if(operation.type == Operation::Remove)
            index += operation.size;
    }
    _operations->insert(i, Operation(this, Operation::Remove, index, 1));
}

// --- Insertable ---

void List::insert(Node *index, Node *item, Node *before, bool *okPtr) {
    Q_UNUSED(before);
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
                    for(int j = 0; j < s; ++j) data.append(operation.oldData.takeAt(0));
                    operation.size -= s;
                    int indexCopy = operation.index;
                    operation.index = index;
                    _operations->insert(i++, Operation(this, Operation::Set, indexCopy, s, data));
                }
                break;
            }
        } else if(operation.type == Operation::Insert) {
            if(index <= operation.index + operation.size) { // I[1..2] + I[1] -> I[1..3]
                operation.oldData.insert(index - operation.index, item);
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
                _operations->insert(i, Operation(this, Operation::Set, index, 1, data));
                return;
            } else
                index += operation.size;
        }
    }
    QList<Node *> data;
    data.append(item);
    _operations->insert(i, Operation(this, Operation::Insert, index, 1, data));
}

int List::_partition(int left, int right, int pivotIndex) {
    int index, store;
    Node *pivot = _get(pivotIndex);
    // move pivot to the end of the array
    Node *tmp = _get(right);
    _set(right, pivot);
    _set(pivotIndex, tmp);
    // all values <= pivot are moved to front of array
    // and pivot inserted just after them
    store = left;
    for(index = left; index < right; index++) {
        Node *current = _get(index);
        if(current->compare(pivot) <= 0) {
            _set(index, _get(store));
            _set(store, current);
            store++;
        }
    }
    tmp = _get(right);
    _set(right, _get(store));
    _set(store, tmp);
    return store;
}

void List::_quickSort(int left, int right) {
    int pivotIndex;
    if(right <= left) return;
    pivotIndex = qrand() % (right - left + 1) + left;
    pivotIndex = _partition(left, right, pivotIndex);
    _quickSort(left, pivotIndex - 1);
    _quickSort(pivotIndex + 1, right);
}

LIU_DEFINE_NATIVE_METHOD(List, sort) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isExclaimed()) {
        sort();
        return this;
    } else {
        List *sortedList = copy();
        sortedList->sort();
        return sortedList;
    }
}

// === List::Operation ===

List::Operation::~Operation() {
    if(_data) {
        foreach(Node *value, *_data) Node::constCast(parent)->removeUnnamedChild(value);
        delete _data;
    }
}

QList<Node *> *List::Operation::data() const {
    if(!_data) const_cast<Operation *>(this)->_data = new QList<Node *>;
    return _data;
}

Node *List::Operation::getData(int index) const {
    return data()->at(index);
}

void List::Operation::setData(int index, Node *value) {
    Node *oldValue = getData(index);
    if(value != oldValue) {
        Node::constCast(parent)->removeUnnamedChild(oldValue);
        data()->replace(index, value);
        Node::constCast(parent)->addUnnamedChild(value);
    }
}

void List::Operation::unsetData(int index) {
    Node::constCast(parent)->removeUnnamedChild(getData(index));
    data()->removeAt(index);
    size--;
}

void List::Operation::insertData(int index, Node *value) {
    data()->insert(index, value);
    Node::constCast(parent)->addUnnamedChild(value);
    size++;
}

// === List::Iterator ===

LIU_DEFINE_2(List::Iterator, Iterable::Iterator, List);

List::Iterator *List::Iterator::init(List *source, int *index) {
    Iterable::Iterator::init();
    setSource(source);
    setIndex(index);
    return this;
}

List::Iterator *List::Iterator::initCopy(const List::Iterator *other) {
    Iterable::Iterator::initCopy(other);
    setSource(other->_source);
    setIndex(other->_index);
    return this;
}

List::Iterator::~Iterator() {
    setSource();
    setIndex();
}

void List::Iterator::initRoot() {
    LIU_ADD_READ_ONLY_PROPERTY(List::Iterator, source)
}

LIU_DEFINE_NODE_ACCESSOR(List::Iterator, List, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(List::Iterator, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(List::Iterator, source);

LIU_DEFINE_ACCESSOR(List::Iterator, int, index, Index, 0);

bool List::Iterator::hasNext() const {
    return index() < source()->size();
}

NodeQPair List::Iterator::peekNext() const {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return NodeQPair(Number::make(index()), source()->_get(index()));
}

void List::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    setIndex(index() + 1);
}

LIU_END
