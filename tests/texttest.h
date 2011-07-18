#ifndef LIU_TEXT_TEST_H
#define LIU_TEXT_TEST_H

#include "node/object/text.h"

#include "tests/testbegin.h"

class TextTest : public QObject {
    Q_OBJECT
private slots:
    void initialize();
    void fork();
    void compare();
};

#include "tests/testend.h"

#endif // LIU_TEXT_TEST_H
