#pragma once
#include "BT_ACTION.h"
#include <string>
#include <BWAPI.h>

class BT_ACTION_ATTACK_ENEMY : public BT_ACTION
{
public:
    BT_ACTION_ATTACK_ENEMY(std::string name, BT_NODE* parent);

private:
    State Evaluate(void* data) override;
    std::string GetDescription() override;
    static BT_NODE::State AttackEnemy(void* data);
    static BWAPI::Unitset GetEnemyUnitsByType(const std::string unitType);
    static BWAPI::Unit GetClosestUnitToSquad(const BWAPI::Unitset& squad, const BWAPI::Unitset& targetList);
};

