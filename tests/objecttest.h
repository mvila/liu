#ifndef LIU_OBJECT_TEST_H
#define LIU_OBJECT_TEST_H

#include "node/object.h"

#include "tests/testbegin.h"

class ObjectTest : public QObject {
    Q_OBJECT
private slots:
    void initialize();
};

#include "tests/testend.h"

#endif // LIU_OBJECT_TEST_H
