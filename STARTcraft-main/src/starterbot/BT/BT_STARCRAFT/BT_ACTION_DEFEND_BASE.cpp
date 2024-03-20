#include "BT_ACTION_DEFEND_BASE.h"
#include "Blackboard.h"
#include <vector>

BT_ACTION_DEFEND_BASE::BT_ACTION_DEFEND_BASE(std::string name, BT_NODE* parent)
    : BT_ACTION(name, parent) {}

BT_NODE::State BT_ACTION_DEFEND_BASE::Evaluate(void* data)
{
    return ReturnState(DefendBase(data));
}

std::string BT_ACTION_DEFEND_BASE::GetDescription()
{
    return "Attacks the enemies that are close to the nexus.";
}


BT_NODE::State BT_ACTION_DEFEND_BASE::DefendBase(void* data)
{
    Blackboard* pData = (Blackboard*)data;

    // get the list of buildings under attack
    std::vector<BWAPI::Unit> buildingsUnderAttack;

    for (const BWAPI::Unit& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType().isBuilding() && unit->isUnderAttack()) {
            buildingsUnderAttack.push_back(unit);
        }
    }
    
    if (buildingsUnderAttack.size() != 0) {
        // move the squads to buildings under attack by distributing them if multiple buildings are under attack 
        short squadIdx = 0;
        for (const BWAPI::Unitset& squad : pData->squads) {
            squad.move(buildingsUnderAttack.at(squadIdx % buildingsUnderAttack.size())->getPosition());
            squadIdx++;
        }
    }

    //std::cout << "Inside DefendBase function\n";

    return BT_NODE::SUCCESS;
}