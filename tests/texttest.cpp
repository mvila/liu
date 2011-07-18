#include "tests/texttest.h"

LIU_BEGIN

void TextTest::initialize() {
    QVERIFY(Text::root()->origin() == Element::root());
    QVERIFY(Object::root()->child("Text") == Text::root());
}

void TextTest::fork() {
    Text *text = LIU_TEXT("Hello");
    QVERIFY(text->origin() == Text::root());
    QCOMPARE(text->value(), QString("Hello"));
    Text *newText(text->fork());
    QVERIFY(newText->origin() == text);
    QCOMPARE(newText->value(), QString("Hello"));
}

void TextTest::compare() {
    Text *t1 = LIU_TEXT("Hello");
    Text *t2 = LIU_TEXT("Hello");
    Text *t3 = LIU_TEXT("Salut");
    QVERIFY(t1->compare(t2) == 0);
    QVERIFY(t1->compare(t3) != 0);
    QVERIFY(t1 != t2);
    QCOMPARE(*t1, *t2);
    QVERIFY(*t1 == *t2);
    QVERIFY(!(*t1 != *t2));
    QVERIFY(*t1 != *t3);
    Text *t4 = LIU_TEXT("Jean");
    Text *t5 = LIU_TEXT("Pierre");
    QVERIFY(*t4 != *t5);
    QCOMPARE(t4->compare(t5), short(-1));
    QCOMPARE(t5->compare(t4), short(1));
}

LIU_END
