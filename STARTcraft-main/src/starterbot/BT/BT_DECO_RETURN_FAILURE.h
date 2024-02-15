#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//The return failure task will always return failure except when the child task is running.
//--------------------
class BT_DECO_RETURN_FAILURE : public BT_DECORATOR {
public:
    BT_DECO_RETURN_FAILURE(std::string name,BT_NODE* parent);
    State Evaluate(void* data) override;
    std::string GetDescription() override;

};
//----------