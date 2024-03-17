#include "AttackGroundUnitBT.h"
#pragma once

AttackGroundUnitBT::AttackGroundUnitBT() {
    // ROOT
    root = new BT_SELECTOR("root", nullptr, 2);

    // STANDARD - START  __________________________________________
    BT_SEQUENCER* standard = new BT_SEQUENCER("defend", root, 3);

    // children of standard node
    BT_CONDITION* isStandard = new BT_CONDITION("isStandard", standard, [](void* data) {
        Blackboard* pData = (Blackboard*)data;
        return pData->gameStatus == GameStatus::Buildup;
        });
    // children of defendChokePoint node
    BT_ACTION_LOG stayInChokePointLog = BT_ACTION_LOG("defendChokePoint", standard, "Stays in the choke point and waits for the enemy.");
    BT_ACTION_STAY_IN_CHOKE_POINT* stayInChokePoint = new BT_ACTION_STAY_IN_CHOKE_POINT("stayInChokePoint", standard);
    // STANDARD - END  ____________________________________________

    // DEFEND - START _____________________________________________
    BT_SEQUENCER* defend = new BT_SEQUENCER("defend", root, 3);

    // children of defend node
    BT_CONDITION* isDefend = new BT_CONDITION("isDefend", defend, [](void* data) {
        Blackboard* pData = (Blackboard*)data;
        return pData->gameStatus == GameStatus::Defence;
        });
    BT_ACTION_LOG attackNearbyEnemyLog = BT_ACTION_LOG("attackNearbyEnemyLog", defend, "Attacks the enemies that are close to the nexus.");
    BT_ACTION_DEFEND_BASE attackNearbyEnemy = BT_ACTION_DEFEND_BASE("attackNearbyEnemy", defend);
    // DEFEND - END _______________________________________________


    // REGROUP - START ____________________________________________
    BT_SEQUENCER* regroup = new BT_SEQUENCER("regroup", root, 2);

    // children of regroup node
    // commented for now since we execute the BTs on unitsets anyway
    /*
    BT_CONDITION* isRegroup = new BT_CONDITION("isRegroup", regroup, [](void* data) {
        AttackGroundUnitBT* instance = static_cast<AttackGroundUnitBT*>(data);
        return instance->commandType == CommandType::REGROUP;
        });
     BT_ACTION_LOG goToPosition = BT_ACTION_LOG("goToPosition", regroup, "Goes to the specified position by commander.");
    */
     // REGROUP - END ______________________________________________


    // ATTACK - START _____________________________________________
    BT_SEQUENCER* attack = new BT_SEQUENCER("attack", root, 3);

    // children of attack node
    BT_CONDITION* isAttack = new BT_CONDITION("isAttack", attack, [](void* data) {
        Blackboard* pData = (Blackboard*)data;
        return pData->gameStatus == GameStatus::Attack;
        });
    BT_ACTION_LOG attackEnemyLog = BT_ACTION_LOG("attackEnemyLog", attack, "Attacks the enemy, first the units and then the buildings.");
    BT_ACTION_ATTACK_ENEMY attackEnemy = BT_ACTION_ATTACK_ENEMY("attackEnemy", attack);
    // ATTACK - END _______________________________________________
}

void AttackGroundUnitBT::Evaluate(void* data) {
    root->Evaluate(data);
}