#include "BasesManager.h";
#include "GameCommander.h";
#include "../BT/Data.h";
#include "../../starterbot/GameFileParser.hpp";

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
void GameCommander::newBase() {

}
