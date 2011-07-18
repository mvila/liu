#ifndef LIU_RUN_STACK_H
#define LIU_RUN_STACK_H

#include "node.h"

LIU_BEGIN

#define LIU_RUN_STACK(ARGS...) new RunStack(context()->child("RunStack"), ##ARGS)

class RunStack : public Node {
    LIU_DECLARE(RunStack, Node, Node);
public:
    explicit RunStack(Node *origin) : Node(origin) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(RunStack);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(RunStack);

    void push(Node *node) {
        _stack.push(node);
    }

    Node *pop() {
        if(_stack.isEmpty()) qFatal("Fatal error: run stack is empty!");
        return _stack.pop();
    }

    Node *top() const {
        if(_stack.isEmpty()) qFatal("Fatal error: run stack is empty!");
        return _stack.top();
    }

    template<class T>
    T *find(bool *okPtr = NULL) const {
        T *run = NULL;
        for (int i = _stack.size() - 1; i > 0; --i) {
            run = T::dynamicCast(_stack.at(i));
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
    QStack<Node *> _stack;
};

inline RunStack *runStack() {
    static RunStack *_stack = new RunStack(RunStack::root());
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
