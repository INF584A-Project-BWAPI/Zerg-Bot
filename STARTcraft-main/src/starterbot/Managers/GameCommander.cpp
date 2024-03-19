#include "BasesManager.h"
#include "GameCommander.h"
#include <Data.h>
#include "GameFileParser.hpp"

void GameCommander::setBuildOrder(std::vector<BuildingRecipe> buildOrder) {
	for (int i = 0; i < buildOrder.size(); i++) {
		const BuildingRecipe order = buildOrder.at(i);
		JobType jobType;

		if (order.getProducer() == ProducerType::Worker) { jobType = JobType::Building; }
		else { jobType = JobType::UnitProduction; };

		const BWAPI::UnitType unit = order.getName();

		JobBase job(i, ManagerType::BaseSupervisor, jobType, false, Importance::Low);
		job.setUnitType(unit);
		job.setGasCost(unit.gasPrice());
		job.setMineralCost(unit.mineralPrice());
		job.setSupplyCost(unit.supplyRequired());

		postJob(job);
	}
}

void GameCommander::onFrame() {
	verifySquadStatuses();
	updateEnemyAttackHitpoints();
	updateOurAttackHitpoints();
	evaluateGameStatus();

	verifySquadOrderStatus();
	addSquadOrders();


	onFrameChildren();
}

void GameCommander::onFrameChildren() {
	distributeJobs();

	basesManager->onFrame();
	scoutManager->onFrame();
	armyManager->onFrame();
}

void GameCommander::distributeJobs() {
	for (const JobBase job : queuedJobs) {
		switch (job.getAssignedManager())
		{
			case ManagerType::BasesManager:
				basesManager->postJob(job);
				break;
			case ManagerType::BaseSupervisor:
				basesManager->postJob(job);
				break;
			case ManagerType::ScoutManager:
				scoutManager->postJob(job);
				break;
			case ManagerType::ArmyManager:
				armyManager->postJob(job);
				break;
			default:
				break;
		}
	}

	// Remove all dsitributed jobs from the queue
	queuedJobs.clear();
}

void GameCommander::evaluateGameStatus() {
	if (blackboard.ourAttackHitpoints > (blackboard.enemyAttackHitpoints + attack_threshold_delta)) {
		blackboard.gameStatus = GameStatus::Attack;
	}
	else {
		for (const BWAPI::Unit& unit : BWAPI::Broodwar->self()->getUnits()) {
			if (unit->getType().isBuilding() && unit->isUnderAttack()) {
				blackboard.gameStatus = GameStatus::Defence;
				break;
			}
		}

		if (blackboard.gameStatus != GameStatus::Defence) {
			blackboard.gameStatus = GameStatus::Standard;
		}
	}
}

void GameCommander::updateOurAttackHitpoints() {
	for (BWAPI::Unitset unitset : blackboard.scout_info) {
		for (const BWAPI::Unit& unit : unitset) {
			if (unit->getType().canAttack() && !blackboard.enemyUnits.contains(unit)) {
				blackboard.enemyAttackHitpoints += unit->getType().maxHitPoints();
				blackboard.enemyUnits.insert(unit);
			}
		}
	}
}

void GameCommander::updateEnemyAttackHitpoints() {
	int ourAttackHitpoints = 0;

	for (BWAPI::Unitset& squad : blackboard.squads) {
		for (const BWAPI::Unit& unit : squad) {
			ourAttackHitpoints += unit->getType().maxHitPoints();
		}
	}

	blackboard.ourAttackHitpoints = ourAttackHitpoints;
}

void GameCommander::verifySquadStatuses() {
	std::vector<BWAPI::Unitset> squadsToRemove;

	for (BWAPI::Unitset& squad : blackboard.squads) {
		std::unordered_set<BWAPI::Unit> unitsToRemove;

		for (const BWAPI::Unit& unit : squad) {
			if (!unit->exists())
				unitsToRemove.insert(unit);
		}

		for (const BWAPI::Unit& unit : unitsToRemove) {
			squad.erase(unit);
		}

		if (squad.empty())
			squadsToRemove.push_back(squad);
	}

	for (const BWAPI::Unitset& squad : squadsToRemove) {
		blackboard.squads.erase(std::remove(blackboard.squads.begin(), blackboard.squads.end(), squad), blackboard.squads.end());
	}
}

void GameCommander::verifySquadOrderStatus() {
	std::vector<int> ordersToErase;

	for (int i = 0; i < blackboard.squadProductionOrders.size(); i++) {
		SquadProductionOrder& squadOrder = blackboard.squadProductionOrders[i];
		if (!squadOrder.isConstructed) {
			bool isFinished = true;

			for (UnitProductionOrder& unitOrder : squadOrder.productionOrder) {
				if (unitOrder.unitInstances.size() < unitOrder.orderCount) {
					isFinished = false;
					break;
				}
			}

			squadOrder.isConstructed = isFinished;

			if (squadOrder.isConstructed) {
				BWAPI::Unitset squad;

				for (UnitProductionOrder& unitOrder : squadOrder.productionOrder) {
					for (BWAPI::Unit unit : unitOrder.unitInstances) {
						squad.insert(unit);
					}
				}

				blackboard.squads.push_back(squad);
				ordersToErase.push_back(i);
			}
		}
	}

	for (int i : ordersToErase) {
		blackboard.squadProductionOrders.erase(blackboard.squadProductionOrders.begin() + i);
	}
}

void GameCommander::addSquadOrders() {
	std::vector<std::string> squadTypes{ "defend", "attack" };

	for (std::string squadType : squadTypes) {
		std::vector<ParsedUnitOrder> orders = gameParser.parseSquadProductionOrders("defend");

		for (SquadProductionOrder order : blackboard.squadProductionOrders) {
			if (order.name == squadType)
				return;
		}

		SquadProductionOrder squadProductionOrder;
		squadProductionOrder.isConstructed = false;
		squadProductionOrder.name = "defend";

		for (ParsedUnitOrder order : orders) {
			UnitProductionOrder unitProductionOrder;
			unitProductionOrder.orderCount = order.count;
			unitProductionOrder.unitType = order.unitType;
			unitProductionOrder.jobsCount = 0;

			squadProductionOrder.productionOrder.push_back(unitProductionOrder);
		}

		blackboard.squadProductionOrders.push_back(squadProductionOrder);
	}
}

