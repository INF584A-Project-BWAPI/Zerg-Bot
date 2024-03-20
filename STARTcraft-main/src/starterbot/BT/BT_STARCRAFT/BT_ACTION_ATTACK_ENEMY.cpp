#include "BT_ACTION_ATTACK_ENEMY.h"
#include "Blackboard.h"

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
        - Find the enemy units and buildings within the range of squad:
            - destroy the units
            - if no units left, destroy buildings
*/
BT_NODE::State BT_ACTION_ATTACK_ENEMY::AttackEnemy(void* data)
{
    Blackboard* pData = (Blackboard*)data;

    // Storing enemy units
    const BWAPI::Unitset& enemyUnits = BWAPI::Broodwar->enemy()->getUnits();

    // Filtering enemy units based on a condition
    const BWAPI::Unitset& enemyArmyUnits = GetEnemyUnitsByType("army");

    // Check if there is any units in the enemy army
    if (enemyArmyUnits.size() != 0) {
        // If there is, iterate over the squads
        for (const BWAPI::Unitset& squad : pData->squads) {
            // Get the closest unit to the squad
            BWAPI::Unit target = GetClosestUnitToSquad(squad, enemyArmyUnits);

            // Attack it
            squad.attack(target);
        }

        return BT_NODE::SUCCESS;
    }

    // If there is no enemy army unit left, do the same for buildings
    const BWAPI::Unitset& enemyBuildings = BT_ACTION_ATTACK_ENEMY::GetEnemyUnitsByType("building");

    for (const BWAPI::Unitset& squad : pData->squads) {
        // Get the closest unit to the squad
        BWAPI::Unit target = GetClosestUnitToSquad(squad, enemyBuildings);

        // Attack it
        squad.attack(target);
    }

    return BT_NODE::SUCCESS;
}

BWAPI::Unitset BT_ACTION_ATTACK_ENEMY::GetEnemyUnitsByType(const std::string unitType)
{
    // Storing enemy units
    const BWAPI::Unitset& enemyUnits = BWAPI::Broodwar->enemy()->getUnits();

    // Filtering enemy units based on a condition
    BWAPI::Unitset filteredEnemyUnits;

    std::copy_if(enemyUnits.begin(), enemyUnits.end(), std::inserter(filteredEnemyUnits, filteredEnemyUnits.end()),
        [&](const BWAPI::Unit& unit) {

            if (unitType == "army") // Filter the units that is a member of the enemy army
                return unit->getType().canAttack();
            else if (unitType == "building") // Filter the units that is an enemy building
                return unit->getType().isBuilding();
        });

    return filteredEnemyUnits;
}

// Returns the closest unit from the input targetList, to the input squad
BWAPI::Unit BT_ACTION_ATTACK_ENEMY::GetClosestUnitToSquad(const BWAPI::Unitset& squad, const BWAPI::Unitset& targetList)
{       
        BWAPI::Unit target = nullptr;
        int minDistance = std::numeric_limits<int>::max();
        for (const BWAPI::Unit& unit : targetList) {
            if (!target || squad.getPosition().getDistance(unit->getPosition()) < minDistance) {
                target = unit;
                minDistance = target->getPosition().getDistance(squad.getPosition());
            }
        }

    return target;
}


