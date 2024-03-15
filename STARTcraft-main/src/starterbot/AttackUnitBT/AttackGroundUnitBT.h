#pragma once

#include "BT_SELECTOR.h"
#include "BT_SEQUENCER.h"
#include "BT_CONDITION.h"
#include "BT_ACTION_LOG.h"
#include "BT_ACTION_IDLE.h"

enum CommandType {
    ATTACK,
    REGROUP,
    DEFEND
};

class AttackGroundUnitBT {
private:
    BT_SELECTOR* root;
    CommandType commandType;

public:
    AttackGroundUnitBT();
    void setCommand(CommandType command);
    CommandType getCommand();
};


