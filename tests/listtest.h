#ifndef LIU_LIST_TEST_H
#define LIU_LIST_TEST_H

#include "node/object/text.h"
#include "node/object/list.h"

#include "tests/testbegin.h"

class ListTest : public QObject {
    Q_OBJECT
private slots:
    void initialize();
    void insertGetAndSet();
    void remove();
    void clear();
};

#include "tests/testend.h"

#endif // LIU_LIST_TEST_H
