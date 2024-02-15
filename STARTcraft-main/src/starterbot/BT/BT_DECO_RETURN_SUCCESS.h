#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//The return success task will always return success except when the child task is running.
// 
//--------------------
class BT_DECO_RETURN_SUCCESS : public BT_DECORATOR {
public:
    BT_DECO_RETURN_SUCCESS(std::string name, BT_NODE* parent);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------