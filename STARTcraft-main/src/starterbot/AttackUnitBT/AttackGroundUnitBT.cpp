#include "AttackGroundUnitBT.h"
#pragma once

AttackGroundUnitBT::AttackGroundUnitBT() {
    // ROOT
    root = new BT_SELECTOR("root", nullptr, 3);

    // DEFEND - START _____________________________________________
    BT_SEQUENCER* defend = new BT_SEQUENCER("defend", root, 2);

    // children of defend node
    BT_CONDITION* isDefend = new BT_CONDITION("isDefend", defend, [](void* data) {
        AttackGroundUnitBT* instance = static_cast<AttackGroundUnitBT*>(data);
        return instance->commandType == CommandType::DEFEND;
        });
    BT_SELECTOR* defendType = new BT_SELECTOR("defendType", defend, 2);

    // children of defendType node
    BT_SEQUENCER* defendYourself = new BT_SEQUENCER("defendYourself", defendType, 3);
    BT_SEQUENCER* defendBase = new BT_SEQUENCER("defendBase", defendType, 3);

    // children of defendYourself node
    BT_CONDITION* isEnemyNearby = new BT_CONDITION("isEnemyNearby", defendYourself, [](void* data) {
        AttackGroundUnitBT* instance = static_cast<AttackGroundUnitBT*>(data);
        return false; // TODO: implement the function
        });
    BT_ACTION_LOG attackNearbyEnemyLog = BT_ACTION_LOG("attackNearbyEnemyLog", defendYourself, "Attacks nearby enemy to defend itself.");

    // children of defendBase node
    BT_CONDITION* isEnemyNearby = new BT_CONDITION("noEnemyNearby", defendBase, [](void* data) {
        AttackGroundUnitBT* instance = static_cast<AttackGroundUnitBT*>(data);
        return false; // TODO: implement the function
        });
    BT_ACTION_LOG stayIdleLog = BT_ACTION_LOG("stayIdleLog", defendBase, "Staying idle in defense mode.");
    BT_ACTION_IDLE* idle = new BT_ACTION_IDLE("idle", defendBase);
    // DEFEND - END _______________________________________________


    // REGROUP - START ____________________________________________
    BT_SEQUENCER* regroup = new BT_SEQUENCER("regroup", root, 2);

    // children of regroup node
    BT_CONDITION* isRegroup = new BT_CONDITION("isRegroup", regroup, [](void* data) {
        AttackGroundUnitBT* instance = static_cast<AttackGroundUnitBT*>(data);
        return instance->commandType == CommandType::REGROUP;
        });
    BT_ACTION_LOG goToPosition = BT_ACTION_LOG("goToPosition", regroup, "Goes to the specified position by commander.");
    // REGROUP - END ______________________________________________


    // ATTACK - START _____________________________________________
    BT_SEQUENCER* attack = new BT_SEQUENCER("attack", root, 2);

    // children of attack node
    BT_CONDITION* isAttack = new BT_CONDITION("isAttack", attack, [](void* data) {
        AttackGroundUnitBT* instance = static_cast<AttackGroundUnitBT*>(data);
        return instance->commandType == CommandType::ATTACK;
        });
    BT_ACTION_LOG attackNearestEnemy = BT_ACTION_LOG("attackNearestEnemy", attack, "Attacks the nearest enemy.");
    // ATTACK - END _______________________________________________
}

void AttackGroundUnitBT::setCommand(CommandType command) {
    commandType = command;
}

CommandType AttackGroundUnitBT::getCommand() {
    return commandType;
}

