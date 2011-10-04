#ifndef LIU_TEXT_H
#define LIU_TEXT_H

#include "node/object/element.h"
#include "node/object/insertable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/character.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Text : public Insertable {
    LIU_DECLARE(Text, Object, Object);
public:
    explicit Text(Node *origin = context()->child("Object", "Text")) : Insertable(origin) {}

    //    Text(const Text &other);

    static Text *make() { return (new Text())->init(); }
    static Text *make(const QString &value) { return (new Text())->init(&value); }

    Text *init(const QString *value = NULL, bool *isTranslatable = NULL, QList<IntPair> *interpolableSlices = NULL);

    virtual ~Text();

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Text);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Text);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(QString, value, Value);
    LIU_DECLARE_NATIVE_METHOD(value_get);
    LIU_DECLARE_NATIVE_METHOD(value_set);

    LIU_DECLARE_ACCESSOR(bool, isTranslatable, IsTranslatable);

    LIU_DECLARE_ACCESSOR(QList<IntPair>, interpolableSlices, InterpolableSlices);

    virtual Node *run(Node *receiver = context());

    virtual bool isEqualTo(const Node *other) const;
    LIU_DECLARE_NATIVE_METHOD(equal_to);

    virtual short compare(const Node *other) const;
    short compare(const QString &other) const;
    LIU_DECLARE_NATIVE_METHOD(compare);

    virtual uint hash() const { return qHash(_value); }

    virtual double toDouble(bool *okPtr = NULL) const;
    virtual QChar toChar() const;
    virtual QString toString(bool debug = false, short level = 0) const;

    // --- Iterable ---

    class Iterator;
    virtual Iterator *iterator() const;
    virtual bool contains(Node *what) const;
    virtual int count(Node *what) const;
    virtual int size() const;
    virtual bool empty() const;

    // --- Collection ---

    virtual void append(Node *item);
    virtual Text *remove(Node *item, bool *wasFoundPtr = NULL);

    // --- Indexable ---

    virtual Character *get(Node *nodeIndex, bool *wasFoundPtr = NULL);
    virtual void set(Node *nodeIndex, Node *nodeValue, bool *wasFoundPtr = NULL);
    virtual void append(Node *nodeIndex, Node *item, bool *okPtr = NULL);
    virtual Character *unset(Node *nodeIndex, bool *wasFoundPtr = NULL);
    class IndexIterator;
    virtual IndexIterator *indexIterator() const;

    // --- Insertable ---

    virtual void insert(Node *nodeIndex, Node *nodeValue, bool *wasFoundPtr = NULL);

    // --- Text ---

    LIU_DECLARE_NATIVE_METHOD(concatenate);
    LIU_DECLARE_NATIVE_METHOD(multiply);

    LIU_DECLARE_NATIVE_METHOD(uppercase);
    LIU_DECLARE_NATIVE_METHOD(lowercase);

    static QString capitalize(QString text);
    LIU_DECLARE_NATIVE_METHOD(capitalize);

    LIU_DECLARE_NATIVE_METHOD(extract_between);
    LIU_DECLARE_NATIVE_METHOD(remove_after);
    LIU_DECLARE_NATIVE_METHOD(replace);

    static QString unescapeSequence(const QString &source, QList<IntPair> *interpolableSlices = NULL);
    static QChar unescapeSequenceNumber(const QString &source, int &i);
private:
    QString *_value;
    bool *_isTranslatable;
    QList<IntPair> *_interpolableSlices;
public:
    // === Iterator ===

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE(Iterator, Object, Text);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "Text", "Iterator")) :
            Iterable::Iterator(origin) {};

        static Iterator *make() { return (new Iterator())->init(); }
        static Iterator *make(Text *text) { return (new Iterator())->init(&text); }

        Iterator *init(Text **text = NULL, int *index = NULL);

        virtual ~Iterator();

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterator);

        LIU_DECLARE_ACCESSOR(TextPtr, text, Text);
        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual Text *peekNext() const;
        virtual void skipNext();
    private:
        Text **_text;
        int *_index;
    };

    // === IndexIterator ===

    class IndexIterator : public Iterable::Iterator {
        LIU_DECLARE(IndexIterator, Object, Text);
    public:
        explicit IndexIterator(Node *origin = context()->child("Object", "Text", "IndexIterator")) :
            Iterable::Iterator(origin) {};

        static IndexIterator *make() { return (new IndexIterator())->init(); }
        static IndexIterator *make(Text *text) { return (new IndexIterator())->init(&text); }

        IndexIterator *init(Text **text = NULL, int *index = NULL);

        virtual ~IndexIterator();

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(IndexIterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(IndexIterator);

        LIU_DECLARE_ACCESSOR(TextPtr, text, Text);
        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual Number *peekNext() const;
        virtual void skipNext();
    private:
        Text **_text;
        int *_index;
    };

    /*
    #define LIU_TEXT_ITERATOR(ARGS...) new Iterator(context()->child("Object", "Text", "Iterator"), ##ARGS)

    #define LIU_CHECK_TEXT_ITERATOR_INDEX \
    if(!hasValue()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");

    class Iterator : public Object {
        LIU_DECLARE(Iterator, Object, Text);
    public:
        explicit Iterator(Node *origin, Text *text = NULL, int index = 0) :
            Object(origin), _text(text), _index(index) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterator, _text, _index);

        LIU_DECLARE_NATIVE_METHOD(init);

        QChar value() const {
            LIU_CHECK_TEXT_ITERATOR_INDEX;
            return _text->value().at(_index);
        }

        LIU_DECLARE_NATIVE_METHOD(value);

        bool hasValue() const {
            if(!_text) LIU_THROW(NullPointerException, "Text pointer is NULL");
            return _index >= 0 && _index < _text->value().size();
        }

        void first() { _index = 0; }
        LIU_DECLARE_NATIVE_METHOD(first);

        void last() {
            if(!_text) LIU_THROW(NullPointerException, "Text pointer is NULL");
            _index = _text->value().size() - 1;
        }

        LIU_DECLARE_NATIVE_METHOD(last);

        void next() { LIU_CHECK_TEXT_ITERATOR_INDEX; _index++; }
        LIU_DECLARE_NATIVE_METHOD(prefix_increment);

        void previous() { LIU_CHECK_TEXT_ITERATOR_INDEX; _index--; }
        LIU_DECLARE_NATIVE_METHOD(prefix_decrement);
    private:
        Text *_text;
        int _index;
    };
*/
};

LIU_END

#endif // LIU_TEXT_H
