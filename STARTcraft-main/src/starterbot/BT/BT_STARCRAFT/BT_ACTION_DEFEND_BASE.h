#pragma once
#include "BT_ACTION.h"
#include <BWAPI.h>

class BT_ACTION_DEFEND_BASE : public BT_ACTION
{
public:
    BT_ACTION_DEFEND_BASE(std::string name, BT_NODE* parent);

private:
    State Evaluate(void* data) override;
    std::string GetDescription() override;
    static BT_NODE::State DefendBase(void* data);
};

