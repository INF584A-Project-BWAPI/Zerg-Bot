#include "ArmyManager.h"
#include <iostream>
#include "Data.h"
#include "AttackGroundUnitBT.h"

void ArmyManager::onFrame() {

    squads = blackboard.squads;

    for (BWAPI::Unitset enemies : blackboard.scout_info) {
        for (auto& enemy : enemies) {
            if (!blackboard.enemyBuilding.contains(enemy) && enemy->getType().isBuilding() && enemy->getType().isResourceDepot()) {
                blackboard.enemyBuilding.insert(enemy);
            }
        }
    }

    if (!squads.empty()) {
        attackGroundUnitBT = new AttackGroundUnitBT();
        if (attackGroundUnitBT->Evaluate(&blackboard) == BT_NODE::SUCCESS) {
            delete (AttackGroundUnitBT*)attackGroundUnitBT;
            attackGroundUnitBT = nullptr;
        }
    }
}
