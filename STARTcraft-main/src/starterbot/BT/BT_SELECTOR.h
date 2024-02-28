#pragma once
#include <cassert>
#include "BT_NODE.h"

//The selector task is similar to an OR boolean operation. It will return success as soon as one of its child tasks return success
//If a child task returns failure then it will sequentially run the next task. If no child task returns success then it will return failure. 
//--------------------
class BT_SELECTOR : public BT_NODE {
private:
    size_t CurrentChildIndex = 0;

public:
    BT_SELECTOR(std::string name, BT_NODE* parent, size_t childrenMaxCount);
    BT_SELECTOR(std::string name, size_t childrenMaxCount);

    State Evaluate(void* data) override;
    void Reset() override;
    std::string GetDescription() override;
};
//----------