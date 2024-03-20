#include "AttackGroundUnitBT.h"
#include <iostream>
#pragma once


AttackGroundUnitBT::AttackGroundUnitBT() noexcept
{
    // ROOT
    root = new BT_SELECTOR("root", nullptr, 3);

    
    // STANDARD - START  __________________________________________
    BT_SEQUENCER* standard = new BT_SEQUENCER("standard", root, 3);

    // children of standard node
    BT_CONDITION* isStandard = new BT_CONDITION("isStandard", standard, [](void* data) {
        Blackboard* pData = (Blackboard*)data;
        return pData->gameStatus == GameStatus::Standard;
        });
    // children of defendChokePoint node
    //BT_ACTION_LOG* stayInChokePointLog = new BT_ACTION_LOG("stayInChokePointLog", standard, "Stays in the choke point and waits for the enemy.");
    BT_ACTION_STAY_IN_CHOKE_POINT* stayInChokePoint = new BT_ACTION_STAY_IN_CHOKE_POINT("stayInChokePoint", standard);
    // STANDARD - END  ____________________________________________
        

    // DEFEND - START _____________________________________________
    BT_SEQUENCER* defend = new BT_SEQUENCER("defend", root, 3);

    // children of defend node
    BT_CONDITION* isDefend = new BT_CONDITION("isDefend", defend, [](void* data) {
        Blackboard* pData = (Blackboard*)data;
        return pData->gameStatus == GameStatus::Defence;
        });
    //BT_ACTION_LOG* attackNearbyEnemyLog = new BT_ACTION_LOG("attackNearbyEnemyLog", defend, "Attacks the enemies that are close to the nexus.");
    BT_ACTION_DEFEND_BASE* attackNearbyEnemy = new BT_ACTION_DEFEND_BASE("attackNearbyEnemy", defend);
    // DEFEND - END _______________________________________________


    // ATTACK - START _____________________________________________
    BT_SEQUENCER* attack = new BT_SEQUENCER("attack", root, 3);

    // children of attack node
    BT_CONDITION* isAttack = new BT_CONDITION("isAttack", attack, [](void* data) {
        Blackboard* pData = (Blackboard*)data;
        return pData->gameStatus == GameStatus::Attack;
        });
    //BT_ACTION_LOG* attackEnemyLog = new BT_ACTION_LOG("attackEnemyLog", attack, "Attacks the enemy, first the units and then the buildings.");
    BT_ACTION_ATTACK_ENEMY* attackEnemy = new BT_ACTION_ATTACK_ENEMY("attackEnemy", attack);
    // ATTACK - END _______________________________________________
}

BT_NODE::State AttackGroundUnitBT::Evaluate(void* data) {
    return root->Evaluate(data);
}
