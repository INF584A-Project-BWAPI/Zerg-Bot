#include "iostream"
#include "Tools.h"
#include <Data.h>
#include "BaseSupervisor.h"

void BaseSupervisor::onFrame() {
    // Build queued buildings
    std::optional<JobBase> jobOptional = peekTopQueuedJob();

    if (jobOptional.has_value()) {
        const JobBase job = *jobOptional;

        switch (job.getJobType())
        {
            case JobType::Building:
                buildBuilding(job);
                break;
            case JobType::UnitProduction:
                produceUnit(job);
                break;
            default:
                break;
        }
    }
}

void BaseSupervisor::postJob(JobBase job) {
    const BWAPI::UnitType unit = job.getUnit();

    queuedJobs.push(job);
}

bool BaseSupervisor::buildBuilding(JobBase job) {
    const BWAPI::UnitType building = job.getUnit();
    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gass = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = building.mineralPrice();
    const int unit_gas = building.gasPrice();

    if (unit_mineral <= excess_mineral && unit_gas <= excess_gass) {
        const bool startedBuilding = Tools::BuildBuilding(building);

        if (startedBuilding) {
            BWAPI::Broodwar->printf("Started Building %s", building.getName().c_str());
            popTopJob();
            postActiveJob(job);

            return true;
        }
    }

    return false;
}

bool BaseSupervisor::produceUnit(JobBase job) {
    const BWAPI::UnitType unitType = job.getUnit();
    const BWAPI::Unit myDepot = Tools::GetDepot();

    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gas = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = unitType.mineralPrice();
    const int unit_gas = unitType.gasPrice();
    
    if (myDepot && !myDepot->isTraining() && unit_mineral <= excess_mineral && unit_gas <= excess_gas) {
        BWAPI::Broodwar->printf("Started Training Unit %s", unitType.getName().c_str());

        myDepot->train(unitType);
        popTopJob();
        postActiveJob(job);

        return true;
    }

    return false;
}
