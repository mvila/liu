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
    if(other->_operations) _operations = other->copyOperations(this);
    return this;
}

List::~List() {
    if(_operations) {
        clearOperations();
        delete _operations;
    }
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
        list->_insert(i, message->runInput(i));
    return list;
}

void List::inspectInternal() const {
    QString str;
    if(!_operations)
        str = "_operations is NULL";
    else {
        str = "[";
        foreach(Operation *operation, *_operations) {
            if(str.size() != 1)
                str += ", ";
            if(operation->type == Operation::Set)
                str += "S";
            else if(operation->type == Operation::Insert)
                str += "I";
            else if (operation->type == Operation::Remove)
                str += "R";
            str += QString("[%1..%2]").arg(operation->index).arg(operation->index + operation->size - 1);
        }
        str += "]";
    }
    P(str.toUtf8());
}

List::Operation *List::getOperation(int index) const {
    if(!_operations) LIU_THROW_NULL_POINTER_EXCEPTION("_operations is NULL");
    return _operations->at(index);
}

void List::unsetOperation(int index) {
    if(!_operations) LIU_THROW_NULL_POINTER_EXCEPTION("_operations is NULL");
    delete _operations->at(index);
    _operations->removeAt(index);
}

void List::insertOperation(int index, Operation *operation) {
    if(!_operations) _operations = new QList<Operation *>;
    _operations->insert(index, operation);
}

void List::clearOperations() {
    if(_operations) {
        foreach(Operation *operation, *_operations) delete operation;
        _operations->clear();
    }
}

int List::countOperations() const {
    return _operations ? _operations->size() : 0;
}

QList<List::Operation *> *List::copyOperations(Node *parent) const {
    if(!_operations) return NULL;
    QList<Operation *> *result = new QList<Operation *>;
    foreach(Operation *operation, *_operations)
        result->append(operation->copy(parent));
    return result;
}

Node *List::unnamedChild(int index) const {
    if(_operations) {
        int i = 0;
        foreach(Operation *operation, *_operations)
            if(operation->type == Operation::Set || operation->type == Operation::Insert) {
                if(index < i + operation->size)
                    return operation->getData(index - i);
                else
                    i += operation->size;
            }
    }
    return NULL;
}

// --- Iterable ---

List::Iterator *List::iterator() const {
    return List::Iterator::make(constCast(this));
}

int List::size() const {
    int result = 0;
    int count = countOperations();
    for(int i = 0; i < count; ++i) {
        Operation *operation = getOperation(i);
        if(operation->type == Operation::Insert)
            result += operation->size;
        else if (operation->type == Operation::Remove)
            result -= operation->size;
    }
    if(List *orig = List::dynamicCast(origin())) result += orig->size();
    return result;
}

// --- Collection ---

void List::append(Node *item) {
    _insertListOrItem(size(), item);
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
    clearOperations();
    int s = size();
    if(s > 0) appendOperation(new Operation(this, Operation::Remove, 0, s));
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
    int count = countOperations();
    if(count > 0) {
        for(int j = 0; j < count; ++j) {
            Operation *operation = getOperation(j);
            if(operation->index > i) break;
            if((operation->type == Operation::Set || operation->type == Operation::Insert)
                    && i < operation->index + operation->size)
                return operation->getData(i - operation->index);
            else if(operation->type == Operation::Insert)
                i -= operation->size;
            else if(operation->type == Operation::Remove)
                i += operation->size;
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
    int i;
    int count = countOperations();
    for(i = 0; i < count; ++i) {
        Operation *operation = getOperation(i);
        if(operation->type == Operation::Set && operation->index - 1 <= index
                && index <= operation->index + operation->size) {
            if(index == operation->index - 1) { // S[1..2] + S[0] -> S[0..2]
                operation->prependData(item);
                operation->index--;
            } else if(index == operation->index + operation->size) { // S[1..2] + S[3] -> S[1..3]
                operation->appendData(item);
                if(i + 1 < count) {
                    Operation *nextOperation = getOperation(i + 1);
                    if(nextOperation->type == Operation::Set && nextOperation->index == index) {
                        if(nextOperation->size > 1) {
                            nextOperation->unsetData(0);
                            nextOperation->index++;
                        } else
                            unsetOperation(i + 1);
                    }
                }
            } else // S[1..2] + S[1] -> S[1..2]
                operation->setData(index - operation->index, item);
            return;
        }
        if(operation->index > index) break;
        if(operation->type == Operation::Insert) {
            if(index < operation->index + operation->size) { // I[1..2] + S[1] -> I[1..2]
                operation->setData(index - operation->index, item);
                return;
            } else
                index -= operation->size;
        } else if(operation->type == Operation::Remove)
            index += operation->size;
    }
    Operation *operation = new Operation(this, Operation::Set, index);
    operation->appendData(item);
    insertOperation(i, operation);
}

void List::append(Node *index, Node *value, bool *okPtr) {
    int max = size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool ok = i == max;
    if(ok) _insertListOrItem(i, value);
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
    int i;
    int count = countOperations();
    for(i = 0; i < count; ++i) {
        Operation *operation = getOperation(i);
        if(operation->type == Operation::Remove &&
                (index == operation->index - 1 || index == operation->index)) {
            if(index == operation->index - 1) operation->index--; // R[1..2] + R[0] -> R[0..2]
            operation->size++; // R[1..2] + R[1] -> R[1..3]
            return;
        }
        if(operation->index > index) break;
        if(operation->type == Operation::Set) {
            if(index < operation->index + operation->size) { // S[1..3] + R[2] -> S[1..1] + R[2..2] + S[3..3]
                int s = index - operation->index;
                Operation *operation2;
                if(s > 0) operation2 = new Operation(this, Operation::Set, operation->index);
                for(int j = 0; j < s; ++j) operation2->appendData(operation->unsetData(0));
                if(operation->size > 1) {
                    operation->unsetData(0);
                    operation->index = index + 1;
                } else
                    unsetOperation(i);
                if(s > 0) insertOperation(i++, operation2);
                break;
            }
        } else if(operation->type == Operation::Insert) { // I[1..3] + R[2] -> I[1..2]
            if(index < operation->index + operation->size) {
                if(operation->size > 1) {
                    operation->unsetData(index - operation->index);
                } else
                    unsetOperation(i);
                return;
            } else
                index -= operation->size;
        } else if(operation->type == Operation::Remove)
            index += operation->size;
    }
    insertOperation(i, new Operation(this, Operation::Remove, index, 1));
}

// --- Insertable ---

void List::insert(Node *index, Node *item, Node *before, bool *okPtr) {
    Q_UNUSED(before);
    int max = size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool ok = i >= 0 && i <= max;
    if(ok) _insertListOrItem(i, item);
    if(okPtr)
        *okPtr = ok;
    else if(!ok) {
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    }
}

void List::_insertListOrItem(int index, Node *listOrItem) {
    List *list = List::dynamicCast(listOrItem);
    if(list && isFlattened()) {
        QScopedPointer<Iterator> i(list->iterator());
        while(i->hasNext()) _insert(index++, i->next().second);
    } else
        _insert(index, listOrItem);
}

void List::_insert(int index, Node *item) {
    int i;
    int count = countOperations();
    for(i = 0; i < count; ++i) {
        Operation *operation = getOperation(i);
        if(operation->index > index) break;
        if(operation->type == Operation::Set) {
            if(index < operation->index + operation->size) {
                if(index > operation->index) { // S[1..3] + I[2] -> S[1] + I[2] + S[2..3]
                    int s = index - operation->index;
                    Operation *operation2 = new Operation(this, Operation::Set, operation->index);
                    for(int j = 0; j < s; ++j) operation2->appendData(operation->unsetData(0));
                    operation->index = index;
                    insertOperation(i++, operation2);
                }
                break;
            }
        } else if(operation->type == Operation::Insert) {
            if(index <= operation->index + operation->size) { // I[1..2] + I[1] -> I[1..3]
                operation->insertData(index - operation->index, item);
                return;
            } else
                index -= operation->size;
        } else if(operation->type == Operation::Remove) {
            if(index == operation->index) { // R[1..2] + I[1] -> S[1] + R[2]
                if(operation->size > 1) {
                    operation->index++;
                    operation->size--;
                } else
                    unsetOperation(i);
                Operation *operation2 = new Operation(this, Operation::Set, index);
                operation2->appendData(item);
                insertOperation(i, operation2);
                return;
            } else
                index += operation->size;
        }
    }
    Operation *operation = new Operation(this, Operation::Insert, index);
    operation->appendData(item);
    insertOperation(i, operation);
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

List::Operation *List::Operation::copy(Node *parent) const {
    Operation *result = new Operation(parent, type, index);
    if(type == Remove)
        result->size = size;
    else if(_data)
        foreach(Node *value, *_data)
            result->appendData(value);
    return result;
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

Node *List::Operation::unsetData(int index) {
    Node *result = getData(index);
    Node::constCast(parent)->removeUnnamedChild(result);
    data()->removeAt(index);
    size--;
    return result;
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
