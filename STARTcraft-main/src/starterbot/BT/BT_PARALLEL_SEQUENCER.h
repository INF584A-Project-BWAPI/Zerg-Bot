#pragma once
#include <cassert>
#include "BT_NODE.h"

//Similar to the sequence task, the parallel task will run each child task until a child task returns failure.
//The difference is that the parallel task will run all of its children tasks simultaneously versus running each task one at a time.
//Like the sequence class, the parallel task will return success once all of its children tasks have return success.
//If one tasks returns failure the parallel task will end all of the child tasks and return failure.
// 
//--------------------
class BT_PARALLEL_SEQUENCER : public BT_NODE {
protected:
    BT_NODE::State* ChildrenStatus;
    virtual ~BT_PARALLEL_SEQUENCER();
public:
    BT_PARALLEL_SEQUENCER(std::string name, BT_NODE* parent, size_t childrenMaxCount);
    BT_PARALLEL_SEQUENCER(std::string name, size_t childrenMaxCount);

    State Evaluate(void* data) override;
    void Reset() override;
    std::string GetDescription() override;
};
//----------