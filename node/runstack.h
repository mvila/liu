#ifndef LIU_RUN_STACK_H
#define LIU_RUN_STACK_H

#include "node.h"

LIU_BEGIN

class RunStack : public Node {
    LIU_DECLARE_2(RunStack, Node, Node);
public:
    explicit RunStack(Node *origin = context()->child("RunStack")) :
        Node(origin), _stack(NULL) {}

    RunStack *init();

    LIU_DECLARE_ACCESSOR(QStack<Node *>, stack, Stack);

    void push(Node *node) {
        stackData()->push(node);
    }

    Node *pop() {
        if(stackData()->isEmpty()) qFatal("Fatal error: run stack is empty!");
        return stackData()->pop();
    }

    Node *top() const {
        if(stack().isEmpty()) qFatal("Fatal error: run stack is empty!");
        return stack().top();
    }

    template<class T>
    T *find(bool *okPtr = NULL) const {
        T *run = NULL;
        for(int i = stack().size() - 1; i > 0; --i) {
            run = T::dynamicCast(stack().at(i));
            if(run) break;
        }
        if(okPtr)
            *okPtr = run;
        else if(!run)
            LIU_THROW_NOT_FOUND_EXCEPTION("no <nodeName> found in run stack"); // TODO: replace <nodeName> with the class name...
        return run;
    }

    void dump() const;
private:
    QStack<Node *> *_stack;
};

inline RunStack *runStack() {
    static RunStack *_stack = RunStack::root()->fork();
    return _stack;
}

#define LIU_PUSH_RUN(NODE) \
RunPusher runPusher(NODE); Q_UNUSED(runPusher);

class RunPusher {
public:
    explicit RunPusher(Node *node) { runStack()->push(node); }
    ~RunPusher() { runStack()->pop(); }
private:
    RunPusher(const RunPusher &); // prevent copying
    RunPusher &operator=(const RunPusher &);
};

LIU_END

#endif // LIU_RUN_STACK_H
