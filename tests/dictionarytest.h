#ifndef LIU_DICTIONARY_TEST_H
#define LIU_DICTIONARY_TEST_H

#include "node/object/text.h"
#include "node/object/dictionary.h"
#include "node/object/list.h"

#include "tests/testbegin.h"

class DictionaryTest : public QObject {
    Q_OBJECT
private slots:
    void initialize();
    void getAndSet();
    void keys();
    void remove();
    void clear();
};

#include "tests/testend.h"

#endif // LIU_DICTIONARY_TEST_H
