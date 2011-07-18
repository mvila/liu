#include "tests/dictionarytest.h"

LIU_BEGIN

void DictionaryTest::initialize() {
    QVERIFY(Dictionary::root()->origin() == Object::root());
    QVERIFY(Object::root()->child("Dictionary") == Dictionary::root());
}

void DictionaryTest::getAndSet() { // TODO: more tests with fork and/or removed key
    Dictionary *dict = LIU_DICTIONARY();
    QCOMPARE(dict->size(), 0);
    QVERIFY(dict->isEmpty());
    QCATCH(dict->get(NULL), NullPointerException);
    QCATCH(dict->get(LIU_TEXT("missing")), NotFoundException);
    QCATCH(dict->set(LIU_TEXT("missing"), NULL), NullPointerException);
    Text *t1 = LIU_TEXT("val1");
    Text *k1 = LIU_TEXT("key1");
    dict->set(k1, t1);
    QCOMPARE(dict->size(), 1);
    QVERIFY(dict->isNotEmpty());
    QVERIFY(dict->get(k1) == t1);
    QVERIFY(dict->get(LIU_TEXT("key1")) == t1);
    QVERIFY(t1->hasDirectParent(dict));
    Text *t2 = LIU_TEXT("val2");
    dict->set(k1, t2);
    QCOMPARE(dict->size(), 1);
    QVERIFY(dict->get(k1) == t2);
    QVERIFY(!t1->hasDirectParent(dict));
    QVERIFY(t2->hasDirectParent(dict));
    dict->set(LIU_TEXT("key1"), t2);
    QCOMPARE(dict->size(), 1);
    QVERIFY(dict->get(k1) == t2);
    QVERIFY(!t1->hasDirectParent(dict));
    QVERIFY(t2->hasDirectParent(dict));
    Text *t3 = LIU_TEXT("val3");
    Text *k2 = LIU_TEXT("key2");
    dict->set(k2, t3);
    QCOMPARE(dict->size(), 2);
    QVERIFY(dict->get(k2) == t3);

    Dictionary *newDict(dict->fork());
    QCOMPARE(newDict->size(), 2);
    QVERIFY(newDict->get(k1)->origin() == t2);
    QVERIFY(newDict->get(k2)->origin() == t3);
}

void DictionaryTest::keys() { // TODO: test with fork and/or removed key
    Dictionary *dict = LIU_DICTIONARY();
    QList<Node::Reference> keys = dict->keys();
    QVERIFY(keys.isEmpty());
    Text *t1 = LIU_TEXT("val1");
    Text *k1 = LIU_TEXT("key1");
    Text *t2 = LIU_TEXT("val2");
    Text *k2 = LIU_TEXT("key2");
    Text *t3 = LIU_TEXT("val3");
    Text *k3 = LIU_TEXT("key3");
    dict->set(k1, t1);
    dict->set(k2, t2);
    dict->set(k3, t3);
    keys = dict->keys();
    QCOMPARE(keys.size(), 3);
    List *list = LIU_LIST();
    foreach (Node::Reference key, keys) list->append(key);
    QVERIFY(list->hasValue(k1));
    QVERIFY(list->hasValue(LIU_TEXT("key1")));
    QVERIFY(list->hasValue(k2));
    QVERIFY(list->hasValue(LIU_TEXT("key2")));
    QVERIFY(list->hasValue(k3));
    QVERIFY(list->hasValue(LIU_TEXT("key3")));
    QVERIFY(!list->hasValue(LIU_TEXT("key4")));
}

void DictionaryTest::remove() { // TODO: test with fork
    Dictionary *dict = LIU_DICTIONARY();
    Text *t1 = LIU_TEXT("val1");
    Text *k1 = LIU_TEXT("key1");
    Text *t2 = LIU_TEXT("val2");
    Text *k2 = LIU_TEXT("key2");
    Text *t3 = LIU_TEXT("val3");
    Text *k3 = LIU_TEXT("key3");
    dict->set(k1, t1);
    dict->set(k2, t2);
    dict->set(k3, t3);
    QCOMPARE(dict->size(), 3);
    QVERIFY(dict->hasKey(k1));
    QVERIFY(dict->get(k1) == t1);
    QVERIFY(t1->hasDirectParent(dict));
    QCATCH(dict->remove(NULL), NullPointerException);
    QCATCH(dict->remove(LIU_TEXT("missing")), NotFoundException);
    dict->remove(k1);
    QCOMPARE(dict->size(), 2);
    QVERIFY(!dict->hasKey(k1));
    QCATCH(dict->get(k1), NotFoundException);
    QVERIFY(!t1->hasDirectParent(dict));
    QCATCH(dict->remove(k1), NotFoundException);
    dict->remove(k2);
    QCOMPARE(dict->size(), 1);
    dict->remove(k3);
    QCOMPARE(dict->size(), 0);
}

void DictionaryTest::clear() {
    Dictionary *dict = LIU_DICTIONARY();
    dict->set(LIU_TEXT("key1"), LIU_TEXT("val1"));
    dict->set(LIU_TEXT("key2"), LIU_TEXT("val2"));
    dict->set(LIU_TEXT("key3"), LIU_TEXT("val3"));
    dict->clear();
    QCOMPARE(dict->size(), 0);
}

LIU_END
