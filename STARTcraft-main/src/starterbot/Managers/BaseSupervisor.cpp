#include "iostream"
#include "Tools.h"
#include <Data.h>
#include "BaseSupervisor.h"

void BaseSupervisor::onFrame() {
    // Build queued buildings
    if (!queuedJobs.isEmpty()) {
        const JobBase& job = queuedJobs.getTop();

        switch (job.getJobType()) {
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

    verifyActiveBuilds();
    verifyFinishedBuilds();
}

bool BaseSupervisor::buildBuilding(const JobBase& job) {
    const BWAPI::UnitType b = job.getUnit();
    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gass = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = b.mineralPrice();
    const int unit_gas = b.gasPrice();

    if (unit_mineral <= excess_mineral && unit_gas <= excess_gass) {
        const auto startedBuilding = buildBuilding(b);
        const BWAPI::TilePosition p = std::get<1>(startedBuilding);
        const int started = std::get<0>(startedBuilding);

        if (started == 1) {
            BWAPI::Broodwar->printf("Moving to Construct Building %s", b.getName().c_str());
            queuedJobs.removeTop();

            Building building(p, b);
            building.status = BuildingStatus::OrderGiven;

            buildings.push_back(building);

            allocated_gas += b.gasPrice();
            allocated_minerals += b.mineralPrice();

            return true;
        }
    }

    return false;
}

bool BaseSupervisor::produceUnit(const JobBase& job) {
    const BWAPI::UnitType unitType = job.getUnit();
    const int buildingIdx = getProductionBuilding(unitType);

    if (buildingIdx == -1) {
        BWAPI::Broodwar->printf("Could Not Find a Building to Produce %s", unitType.getName().c_str());
        return false;
    }

    const BWAPI::Unit building = buildings.at(buildingIdx).unit;

    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gas = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = unitType.mineralPrice();
    const int unit_gas = unitType.gasPrice();
    
    if (building && !building->isTraining() && unit_mineral <= excess_mineral && unit_gas <= excess_gas) {
        BWAPI::Broodwar->printf("Started Training Unit %s", unitType.getName().c_str());

        building->train(unitType);
        queuedJobs.removeTop();

        return true;
    }

    return false;
}

void BaseSupervisor::verifyActiveBuilds() {
    for (const BWAPI::Unit& buildingStarted : BWAPI::Broodwar->getAllUnits()) {
        if (!buildingStarted->getType().isBuilding() || !buildingStarted->isBeingConstructed()) {
            continue;
        }

        for (Building& building : buildings) {
            if (building.status == BuildingStatus::OrderGiven) {
                const float dx = building.position.x - buildingStarted->getTilePosition().x;
                const float dy = building.position.y - buildingStarted->getTilePosition().y;

                if (dx * dx + dy * dy == 0.0) {
                    allocated_gas -= building.unitType.gasPrice();
                    allocated_minerals -= building.unitType.mineralPrice();

                    BWAPI::Broodwar->printf("Started Constructing: %s", building.unitType.getName().c_str());
                    //buildings.erase(std::remove(buildings.begin(), buildings.end(), building), buildings.end());
                    building.status = BuildingStatus::UnderConstruction;

                    break;
                }
            }
        }
    }
}

void BaseSupervisor::verifyFinishedBuilds() {
    for (const BWAPI::Unit& buildingInstance : BWAPI::Broodwar->getAllUnits()) {
        if (!buildingInstance->getType().isBuilding() || buildingInstance->isBeingConstructed()) {
            continue;
        }

        for (Building& building : buildings) {
            if (building.status == BuildingStatus::UnderConstruction || building.status == BuildingStatus::OrderGiven) {
                building.status = BuildingStatus::Constructed;
                building.unit = buildingInstance;
            }
        }
    }
}

// Helper methods

std::tuple<int, BWAPI::TilePosition> BaseSupervisor::buildBuilding(BWAPI::UnitType b)
{
    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();
    const BWAPI::UnitType builderType = b.whatBuilds().first;

    BWAPI::Unit builder = Tools::GetUnitOfType(builderType);
    if (!builder) { return std::make_tuple(-1, desiredPos); }

    // Ask BWAPI for a building location near the desired position for the type
    constexpr int maxBuildRange = 64;
    const bool buildingOnCreep = b.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(b, desiredPos, maxBuildRange, buildingOnCreep);
    
    const bool orderGiven = builder->build(b, buildPos);

    if (orderGiven) {
        return std::make_tuple(1, buildPos);
    }
    else {
        return std::make_tuple(0, buildPos);
    }
}

int BaseSupervisor::getProductionBuilding(BWAPI::UnitType u) {
    for (int i = 0; i < buildings.size(); i++) {
        BWAPI::UnitType::set canProduce = buildings.at(i).unitType.buildsWhat();

        if (canProduce.contains(u)) {
            return i;
        }
    }

    return -1;
}

