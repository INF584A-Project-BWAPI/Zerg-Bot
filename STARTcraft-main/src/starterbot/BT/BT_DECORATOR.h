#pragma once
#include <cassert>
#include "BT_NODE.h"


// Base decorator class (a node with only 1 child)
//--------------------
class BT_DECORATOR : public BT_NODE {
public:
    BT_DECORATOR(std::string name,BT_NODE* parent);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------