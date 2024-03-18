#pragma once
#include "BT_ACTION.h"
class BT_ACTION_STAY_IN_CHOKE_POINT : public BT_ACTION
{
public:
    BT_ACTION_STAY_IN_CHOKE_POINT(std::string name, BT_NODE* parent);

private:
    State Evaluate(void* data) override;
    std::string GetDescription() override;
    static BT_NODE::State StayInChokePoint(void* data);
};

