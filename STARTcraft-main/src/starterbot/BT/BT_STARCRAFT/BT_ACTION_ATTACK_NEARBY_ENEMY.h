#include "BT_ACTION.h"
#include "BT_NODE.h"


#pragma once
class BT_ACTION_ATTACK_NEARBY_ENEMY : public BT_ACTION {
public:
    BT_ACTION_ATTACK_NEARBY_ENEMY(std::string name, BT_NODE* parent);

private:
    State Evaluate(void* data) override;
    std::string GetDescription() override;
    static State AttackNearbyEnemy(void* data);
};

