#include "node/object/text.h"
#include "node/object/comparable.h"
#include "node/object/language/interpreter.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Text ===

LIU_DEFINE_2(Text, Object, Object);

Text *Text::init(const QString *value, bool *isTranslatable, QList<IntPair> *interpolableSlices) {
    Insertable::init();
    setValue(value);
    setIsTranslatable(isTranslatable);
    setInterpolableSlices(interpolableSlices);
    return this;
}

Text *Text::initCopy(const Text *other) {
    Insertable::initCopy(other);
    setValue(other->_value);
    setIsTranslatable(other->_isTranslatable);
    setInterpolableSlices(other->_interpolableSlices);
    return this;
}

Text::~Text() {
    setValue();
    setIsTranslatable();
    setInterpolableSlices();
}

void Text::initRoot() {
    addExtension(Comparable::root());
    addExtension(Insertable::root());

    LIU_ADD_NATIVE_METHOD(Text, init);

    LIU_ADD_PROPERTY(Text, value);

    LIU_ADD_NATIVE_METHOD(Text, concatenate, +);
    LIU_ADD_NATIVE_METHOD(Text, multiply, *);

    LIU_ADD_NATIVE_METHOD(Text, uppercase);
    LIU_ADD_NATIVE_METHOD(Text, lowercase);
    LIU_ADD_NATIVE_METHOD(Text, capitalize);

    LIU_ADD_NATIVE_METHOD(Text, extract_between);
    LIU_ADD_NATIVE_METHOD(Text, remove_after);
    LIU_ADD_NATIVE_METHOD(Text, replace);
}

LIU_DEFINE_NATIVE_METHOD(Text, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasAnInput()) setValue(message->runFirstInput()->toString());

//    // === TODO: DRY ===
//    LIU_FIND_LAST_PRIMITIVE;
//    Primitive *nextPrimitive = primitive->hasNext();
//    if(nextPrimitive) {
//        nextPrimitive->run(this);
//        Primitive::skip(this);
//    }

    return this;
}

LIU_DEFINE_ACCESSOR(Text, QString, value, Value,);

LIU_DEFINE_NATIVE_METHOD(Text, value_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(Text::cast(parent())->hasValue());
    else
        return parent();
}

LIU_DEFINE_NATIVE_METHOD(Text, value_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Text::cast(parent())->setValue(message->runFirstInput()->toString());
    return this;
}

LIU_DEFINE_ACCESSOR(Text, bool, isTranslatable, IsTranslatable, true);

LIU_DEFINE_ACCESSOR(Text, QList<IntPair>, interpolableSlices, InterpolableSlices,);

bool Text::isDefined(QSet<const Node *> *alreadySeen) const {
    return hasValue() || hasIsTranslatable() || hasInterpolableSlices() || Object::isDefined(alreadySeen);
}

Node *Text::run(Node *receiver) {
    Q_UNUSED(receiver);
    QString result = value();
    if(hasInterpolableSlices()) {
        LIU_PUSH_RUN(this);
        QString source;
        QString str;
        int offset = 0;
        foreach (IntPair slice, interpolableSlices()) {
            source = result.mid(offset + slice.first + 1, slice.second - 2);
            str = !source.isEmpty() ? Interpreter::root()->runSourceCode("liu:" + source)->toString() : "";
            result.replace(offset + slice.first, slice.second, str);
            offset += str.size() - slice.second;
        }
    }
    return Text::make(result);
}

bool Text::isEqualTo(const Node *other) const {
    const Text *otherText = Text::dynamicCast(other);
    if(otherText) return value() == otherText->value();
    const Character *otherCharacter = Character::dynamicCast(other);
    if(otherCharacter) return value() == otherCharacter->toString();
    return false;
}

short Text::compare(const Node *other) const {
    const Text *otherText = Text::dynamicCast(other);
    if(otherText) return compare(otherText->value());
    const Character *otherCharacter = Character::dynamicCast(other);
    if(otherCharacter) return compare(otherCharacter->toString());
    LIU_THROW(ArgumentException, "a Text or a Character is expected");
}

short Text::compare(const QString &other) const {
    int result = value().compare(other);
    if(result > 0) return 1;
    else if(result < 0) return -1;
    else return 0;
}

double Text::toDouble(bool *okPtr) const {
    bool ok;
    double number = value().toDouble(&ok);
    if(okPtr)
        *okPtr = ok;
    else if(!ok)
        LIU_THROW_CONVERSION_EXCEPTION("conversion from Text to Number failed");
    return ok ? number : 0;
};

QChar Text::toChar(bool *okPtr) const {
    bool ok = value().size() == 1;
    if(okPtr)
        *okPtr = ok;
    else if(!ok)
        LIU_THROW_CONVERSION_EXCEPTION("conversion from Text to Character failed (size should be equal to 1)");
    return ok ? value().at(0) : QChar();
};

QString Text::toString(bool debug, short level) const {
    Q_UNUSED(level);
    return debug ? "\"" + value() + "\"" : value();
}

// --- Iterable ---

Text::Iterator *Text::iterator() const {
    return Text::Iterator::make(constCast(this));
}

int Text::size() const {
    return value().size();
}

bool Text::isEmpty() const {
    return value().isEmpty();
}

bool Text::contains(Node *what) const {
    return value().contains(what->toString());
}

int Text::count(Node *what) const {
    return value().count(what->toString());
}

// --- Collection ---

void Text::append(Node *item) {
    setValue(value() + item->toString());
}

Text *Text::remove(Node *item, bool *wasFoundPtr) {
    QString source = value();
    QString str = item->toString();
    int index = source.indexOf(str);
    bool wasFound = index != -1;
    Text *result = NULL;
    if(wasFound) {
        result = Text::make(source.mid(index, str.size()));
        source.remove(index, str.size());
        setValue(source);
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(NotFoundException, "value not found");
    return result;
}

void Text::clear() {
    setValue(QString());
}

// --- Indexable ---

Character *Text::get(Node *index, bool *wasFoundPtr) {
    int i = index->toDouble();
    int max = value().size();
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i < max;
    Character *result = wasFound ? Character::make(value().at(i)) : NULL;
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    return result;
}

//LIU_DEFINE_NATIVE_METHOD(Text, get) {
//    LIU_FIND_LAST_MESSAGE;
//    LIU_CHECK_INPUT_SIZE(1, 2);
//    int index = message->runFirstInput()->toDouble();
//    int max = value().size();
//    if(index < 0) index = max + index;
//    if(index < 0 || index >= max) LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
//    int size = message->hasASecondInput() ? message->runSecondInput()->toDouble() : 1;
//    if(size < 0) size = max + (size + 1) - index;
//    if(size < 0 || index + size > max) LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
//    return Text::make(value().mid(index, size));
//}

void Text::set(Node *index, Node *item, bool *wasFoundPtr) {
    int i = index->toDouble();
    QString str = value();
    int max = str.size();
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i < max;
    if(wasFound) {
        str.remove(i, 1);
        str.insert(i, item->toString());
        setValue(str);
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
}

void Text::append(Node *index, Node *item, bool *okPtr) {
    QString str = value();
    int max = str.size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool ok = i == max;
    if(ok) setValue(value() + item->toString());
    if(okPtr)
        *okPtr = ok;
    else if(!ok) {
        if(i < max)
            LIU_THROW(DuplicateException, "index already exists");
        else
            LIU_THROW(IndexOutOfBoundsException, "index is invalid");
    }
}

Character *Text::unset(Node *index, bool *wasFoundPtr) {
    int i = index->toDouble();
    QString str = value();
    int max = str.size();
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i < max;
    Character *result = NULL;
    if(wasFound) {
        result = Character::make(str.at(i));
        str.remove(i, 1);
        setValue(str);
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    return result;
}

// --- Insertable ---

void Text::insert(Node *index, Node *item, Node *before, bool *wasFoundPtr) {
    Q_UNUSED(before);
    QString str = value();
    int max = str.size();
    int i = index ? index->toDouble() : max;
    if(i < 0) i = max + i;
    bool wasFound = i >= 0 && i <= max;
    if(wasFound) {
        str.insert(i, item->toString());
        setValue(str);
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
}

// --- Text ---

LIU_DEFINE_NATIVE_METHOD(Text, concatenate) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Text::make(value() + message->runFirstInput()->toString());
}

LIU_DEFINE_NATIVE_METHOD(Text, multiply) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Text::make(value().repeated(message->runFirstInput()->toDouble()));
}

LIU_DEFINE_NATIVE_METHOD(Text, uppercase) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isExclaimed())
        return Text::make(value().toUpper());
    else {
        setValue(value().toUpper());
        return this;
    }
}

LIU_DEFINE_NATIVE_METHOD(Text, lowercase) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isExclaimed())
        return Text::make(value().toLower());
    else {
        setValue(value().toLower());
        return this;
    }
}

QString Text::capitalize(QString text) {
    if(!text.isEmpty()) text[0] = text[0].toUpper();
    return text;
}

LIU_DEFINE_NATIVE_METHOD(Text, capitalize) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isExclaimed())
        return Text::make(capitalize(value()));
    else {
        setValue(capitalize(value()));
        return this;
    }
}

LIU_DEFINE_NATIVE_METHOD(Text, extract_between) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    QString text = value();
    QString after = message->runFirstInput()->toString();
    QString before = message->runSecondInput()->toString();
    int from = 0;
    if(!after.isEmpty()) {
        from = text.indexOf(after);
        if(from == -1) return Boolean::make(false);
    }
    int to;
    if(!before.isEmpty()) {
        to = text.indexOf(before, from);
        if(to == -1) return Boolean::make(false);
        to += before.size() - 1;
    } else {
        to = text.size() - 1;
    }
    Node *result = Text::make(text.mid(from + after.size(),
                         (to - before.size()) - (from + after.size()) + 1));
    if(message->isExclaimed())
        setValue(text.remove(from, to - from + 1));
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Text, remove_after) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    QString text = value();
    QString after = message->runFirstInput()->toString();
    if(!after.isEmpty()) {
        int index = text.indexOf(after);
        if(index != -1) text.remove(index, text.size() - index);
    }
    if(message->isExclaimed()) {
        setValue(text);
        return this;
    } else
        return Text::make(text);
}

LIU_DEFINE_NATIVE_METHOD(Text, replace) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    QString oldText = message->runFirstInput()->toString();
    if(oldText.isEmpty()) LIU_THROW(ArgumentException, "first argument is empty");
    QString newText = message->runSecondInput()->toString();
    QString source = value();
    source.replace(oldText, newText);
    if(message->isExclaimed()) {
        setValue(source);
        return this;
    } else
        return Text::make(source);
}

QString Text::unescapeSequence(const QString &source, QList<IntPair> *interpolableSlices) {
    QString result;
    int i = 0;
    QChar c;
    while(i < source.size()) {
        c = source.at(i);
        if(c == '{' && interpolableSlices) {
            int end = source.indexOf('}', i + 1);
            if(end == -1) LIU_THROW(RuntimeException, "'}' not found after '{'");
            interpolableSlices->append(IntPair(result.size(), end - i + 1));
            result += source.mid(i, end - i + 1);
            i = end + 1;
        } else {
            if(c == '\\') {
                i++;
                if(i == source.size()) LIU_THROW(RuntimeException, "invalid escape sequence: '\\'");
                c = source.at(i);
                switch(c.toAscii()) {
                case 't': c = '\t';  break;
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case '"': c = '"'; break;
                case '\'': c = '\''; break;
                case '{': c = '{'; break;
                case '\\': c = '\\'; break;
                default:
                    if(QString("01234567xu").contains(c, Qt::CaseInsensitive))
                        c = Text::unescapeSequenceNumber(source, i);
                    else
                        LIU_THROW(RuntimeException, QString("unknown escape sequence: '\\%1'").arg(c));
                }
            }
            result += c;
            i++;
        }
    }
    return result;
}

QChar Text::unescapeSequenceNumber(const QString &source, int &i) {
    char type;
    QString allowedChars;
    short maxSize;
    QChar c = source.at(i);
    if(c == 'x' || c == 'X') {
        type = 'x';
        allowedChars = "0123456789abcdef";
        maxSize = 2;
    } else if(c == 'u' || c == 'U') {
        type = 'u';
        allowedChars = "0123456789abcdef";
        maxSize = 4;
    } else {
        type = 'o';
        allowedChars = "01234567";
        maxSize = 3;
        i--;
    }
    QString number = "";
    do {
        i++;
        if(i == source.size()) LIU_THROW(RuntimeException, "invalid escape sequence number");
        c = source.at(i);
        if(!allowedChars.contains(c, Qt::CaseInsensitive)) { i--; break; }
        number += c;
    } while(number.size() < maxSize);
    if(number.isEmpty()) LIU_THROW(RuntimeException, "invalid escape sequence number");
    bool ok;
    ushort code = type == 'o' ? number.toUShort(&ok, 8) : number.toUShort(&ok, 16);
    if(!ok || (type != 'u' && code > 0xFF)) LIU_THROW(RuntimeException, "invalid number in escape sequence");
    return QChar(code);
}

// === Text::Iterator ===

LIU_DEFINE_2(Text::Iterator, Iterable::Iterator, Text);

Text::Iterator *Text::Iterator::init(Text *source, int *index) {
    Iterable::Iterator::init();
    setSource(source);
    setIndex(index);
    return this;
}

Text::Iterator *Text::Iterator::initCopy(const Text::Iterator *other) {
    Iterable::Iterator::initCopy(other);
    setSource(other->_source);
    setIndex(other->_index);
    return this;
}

Text::Iterator::~Iterator() {
    setSource();
    setIndex();
}

void Text::Iterator::initRoot() {
    setSource(Text::root());

    LIU_ADD_READ_ONLY_PROPERTY(Text::Iterator, source)
}

LIU_DEFINE_NODE_ACCESSOR(Text::Iterator, Text, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Text::Iterator, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(Text::Iterator, source);

LIU_DEFINE_ACCESSOR(Text::Iterator, int, index, Index, 0);

bool Text::Iterator::hasNext() const {
    return index() < source()->value().size() ;
}

NodeQPair Text::Iterator::peekNext() const {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return NodeQPair(Number::make(index()), Text::make(source()->value().at(index())));
}

void Text::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    setIndex(index() + 1);
}

LIU_END
