#ifndef LIU_TEXT_H
#define LIU_TEXT_H

#include "node/object/insertable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/character.h"
#include "node/object/property.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Text : public Insertable {
    LIU_DECLARE_2(Text, Object, Object);
public:
    explicit Text(Node *origin = context()->child("Object", "Text")) :
        Insertable(origin), _value(NULL), _isTranslatable(NULL), _interpolableSlices(NULL) {}

    static Text *make(const QString &value) { return (new Text())->init(&value); }

    Text *init(const QString *value = NULL, bool *isTranslatable = NULL, QList<IntPair> *interpolableSlices = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(QString, value, Value);
    LIU_DECLARE_PROPERTY(value);

    LIU_DECLARE_ACCESSOR(bool, isTranslatable, IsTranslatable);

    LIU_DECLARE_ACCESSOR(QList<IntPair>, interpolableSlices, InterpolableSlices);

    virtual bool isDefined(QSet<const Node *> *alreadySeen = NULL) const;

    virtual Node *run(Node *receiver = context());

    virtual bool isEqualTo(const Node *other) const;

    virtual short compare(const Node *other) const;
    short compare(const QString &other) const;

    virtual uint hash() const { return qHash(value()); }

    virtual double toDouble(bool *okPtr = NULL) const;
    virtual QChar toChar(bool *okPtr = NULL) const;
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
    virtual void clear();

    // --- Indexable ---

    virtual Character *get(Node *index, bool *wasFoundPtr = NULL);
    virtual void set(Node *index, Node *item, bool *wasFoundPtr = NULL);
    virtual void append(Node *index, Node *item, bool *okPtr = NULL);
    virtual Character *unset(Node *index, bool *wasFoundPtr = NULL);
    virtual bool indexIsSequential() const { return true; }

    // --- Insertable ---

    virtual void insert(Node *index, Node *item, Node *before = NULL, bool *wasFoundPtr = NULL);

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
        LIU_DECLARE_2(Iterator, Iterable::Iterator, Text);
    public:
        explicit Iterator(Node *origin = context()->child("Object", "Text", "Iterator")) :
            Iterable::Iterator(origin), _source(NULL), _index(NULL) {};

        static Iterator *make(Text *source) { return (new Iterator())->init(source); }

        Iterator *init(Text *source = NULL, int *index = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Text, source, Source);
        LIU_DECLARE_READ_ONLY_PROPERTY(source);

        LIU_DECLARE_ACCESSOR(int, index, Index);

        virtual bool hasNext() const;
        virtual NodeQPair peekNext() const;
        virtual void skipNext();
    private:
        Text *_source;
        int *_index;
    };
};

LIU_END

#endif // LIU_TEXT_H
