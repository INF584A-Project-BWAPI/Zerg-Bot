#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//The until failure task will keep executing its child task until the child task returns failure.
// 
//--------------------
class BT_DECO_UNTIL_FAILURE : public BT_DECORATOR {
public:
    BT_DECO_UNTIL_FAILURE(std::string name, BT_NODE* parent);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------