#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//The until success task will keep executing its child task until the child task returns success.
//--------------------
class BT_DECO_UNTIL_SUCCESS : public BT_DECORATOR {
public:
    BT_DECO_UNTIL_SUCCESS(std::string name, BT_NODE* parent);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------