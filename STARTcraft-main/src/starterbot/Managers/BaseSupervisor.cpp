#include "iostream"
#include "Tools.h"
#include <Data.h>
#include "BaseSupervisor.h"

void BaseSupervisor::onFrame() {
    //std::cout << "BuildSupervisor: onFrame has been called!" << std::endl;

    // Build queued buildings
    std::optional<BuildJob> buildJob = peekBuildJob();

    if (buildJob.has_value()) {
        const BuildJob job = *buildJob;
        const bool started = buildBuilding(job);
    }

    // Produce queued unit production
    std::optional<UnitProduceJob> produceJob = peekUnitProduceJob();

    if (produceJob.has_value()) {
        const UnitProduceJob job = *produceJob;
        const bool started = produceUnit(job);
    }
}

void BaseSupervisor::postJob(BuildJob job) {
    allocated_crystal += job.getBuildingType().mineralPrice();
    allocated_gas += job.getBuildingType().gasPrice();

    queuedBuildJobs.push(job);
}

void BaseSupervisor::postJob(UnitProduceJob job) {
    allocated_crystal += job.getUnitType().mineralPrice();
    allocated_gas += job.getUnitType().gasPrice();

    queuedUnitProduceJobs.push(job);
}

bool BaseSupervisor::buildBuilding(BuildJob job)
{
    const BWAPI::UnitType building = job.getBuildingType();
    int excess_crystal = BWAPI::Broodwar->self()->minerals() - allocated_crystal;
    int excess_gass = BWAPI::Broodwar->self()->gas() - allocated_gas;

    if (job.getRequiredCrystal() <= excess_crystal && job.getRequiredGas() <= excess_gass) {
        const bool startedBuilding = Tools::BuildBuilding(building);

        if (startedBuilding) {
            BWAPI::Broodwar->printf("Started Building %s", building.getName().c_str());
            removeTopBuildJob();
            postActiveJob(job);

            allocated_crystal -= job.getRequiredCrystal();
            allocated_gas -= job.getRequiredGas();

            return true;
        }
    }

    return false;
}

bool BaseSupervisor::produceUnit(UnitProduceJob job)
{
    const BWAPI::UnitType unitType = job.getUnitType();
    const BWAPI::Unit myDepot = Tools::GetDepot();

    int excess_crystal = BWAPI::Broodwar->self()->minerals() - allocated_crystal;
    int excess_gass = BWAPI::Broodwar->self()->gas() - allocated_gas;

    int unit_crystal = unitType.mineralPrice();
    int unit_gas = unitType.gasPrice();
    
    if (myDepot && !myDepot->isTraining() && unit_crystal <= excess_crystal && unit_gas <= excess_gass) {
        BWAPI::Broodwar->printf("Started Training Unit %s", unitType.getName().c_str());

        myDepot->train(unitType);
        removeTopUnitProduceJob();
        postActiveJob(job);

        allocated_crystal -= unitType.mineralPrice();
        allocated_gas -= unitType.gasPrice();

        return true;
    }

    return false;
}
