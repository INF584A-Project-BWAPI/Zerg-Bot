#pragma once
#include <cassert>
#include "BT_NODE.h"

//Similar to the selector task, the parallel selector task will return success as soon as a child task returns success.
//The difference is that the parallel task will run all of its children tasks simultaneously versus running each task one at a time.
//If one tasks returns success the parallel selector task will end all of the child tasks and return success.
//If every child task returns failure then the parallel selector task will return failure.
// 
//--------------------
class BT_PARALLEL_SELECTOR : public BT_NODE {
protected:
    BT_NODE::State* ChildrenStatus;
    virtual ~BT_PARALLEL_SELECTOR();
public:
    BT_PARALLEL_SELECTOR(std::string name, BT_NODE* parent, size_t childrenMaxCount);
    BT_PARALLEL_SELECTOR(std::string name, size_t childrenMaxCount);

    State Evaluate(void* data) override;
    void Reset() override;
    std::string GetDescription() override;
};
//----------