#include "BT_ACTION_ATTACK_ENEMY.h"
#include "Tools.h"
#include "Data.h"

BT_ACTION_ATTACK_ENEMY::BT_ACTION_ATTACK_ENEMY(std::string name, BT_NODE* parent)
    : BT_ACTION(name, parent) {}

BT_NODE::State BT_ACTION_ATTACK_ENEMY::Evaluate(void* data)
{
    return ReturnState(AttackEnemy(data));
}

std::string BT_ACTION_ATTACK_ENEMY::GetDescription()
{
    return "Attacks the enemy, first the units and then the buildings.";
}


/*
    A naive attacking approach:
        Find the enemy units and buildings within the range of squad:
            - destroy the units
            - if no units left, destroy buildings
*/
BT_NODE::State BT_ACTION_ATTACK_ENEMY::AttackEnemy(void* data)
{
    Data* pData = (Data*)data;

    return BT_NODE::SUCCESS;
}