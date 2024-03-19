#pragma once

#include "BT_SELECTOR.h"
#include "BT_SEQUENCER.h"
#include "BT_CONDITION.h"
#include "BT_ACTION_LOG.h"
#include "BT_ACTION_IDLE.h"
#include "BT_ACTION_ATTACK_ENEMY.h"
#include "BT_ACTION_STAY_IN_CHOKE_POINT.h"
#include "BT_ACTION_DEFEND_BASE.h"
#include "Blackboard.h"

class AttackGroundUnitBT {
private:
    BT_SELECTOR* root;

public:
    AttackGroundUnitBT(void) noexcept;
    BT_NODE::State Evaluate(void* data);
};


