#include "BT_ACTION_ATTACK_NEARBY_ENEMY.h"

BT_NODE::State BT_ACTION_ATTACK_NEARBY_ENEMY::Evaluate(void* data) {
	return ReturnState(AttackNearbyEnemy(data));
};

std::string BT_ACTION_ATTACK_NEARBY_ENEMY::GetDescription(){
	return "Attacks nearby enemy to defend itself.";
}

BT_NODE::State BT_ACTION_ATTACK_NEARBY_ENEMY::AttackNearbyEnemy(void* data)
{
	return BT_NODE::State::SUCCESS;
}
