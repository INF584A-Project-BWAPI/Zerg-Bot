#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//The inverter task will invert the return value of the child task after it has finished executing.
//If the child returns success, the inverter task will return failure. If the child returns failure, the inverter task will return success.
// 
//--------------------
class BT_DECO_INVERTER : public BT_DECORATOR {
public:
    BT_DECO_INVERTER(std::string name, BT_NODE* parent);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------