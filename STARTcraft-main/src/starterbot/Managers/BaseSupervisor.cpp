#include "iostream"
#include "../Tools.h"
#include "../BT/Data.h"
#include "BaseSupervisor.h"
#include "../BT/BT.h"

void BaseSupervisor::onFrame() {
    // Build queued buildings
    if (!queuedJobs.isEmpty()) {
        
        const JobBase& job = queuedJobs.getTop();
        bool doNotSkip = true;

        switch (job.getJobType()) {
            case JobType::Building:
                doNotSkip = buildBuilding(job);
                break;
            case JobType::UnitProduction:
                doNotSkip = produceUnit(job);
                break;
            case JobType::UpgradeJob:
                doNotSkip = startUpgrade(job);
                break;
            default:
                break;
        }
    }

    verifyActiveBuilds();
    verifyFinishedBuilds();
    assignIdleWorkes();
    assignWorkersToGas();
    pDataResources->unitsAvailable = workers;
    pDataResources->currMinerals = BWAPI::Broodwar->self()->minerals();

    if (pBT != nullptr && pBT->Evaluate(pDataResources) != BT_NODE::RUNNING) {
        delete (BT_DECORATOR*)pBT;
        pBT = nullptr;
    }
}

bool BaseSupervisor::startUpgrade(const JobBase& job) {
    // Find a building that can perform the upgrade
    BWAPI::UpgradeType upgradeType=job.getUpgrade();
    for (auto& building : buildings) {
        if (building.unit->getType().upgradesWhat().contains(upgradeType)) {
            // Check if the upgrade can be started (e.g., enough resources, prerequisites met)
            if (BWAPI::Broodwar->self()->isUpgrading(upgradeType) || building.unit->upgrade(upgradeType)) {
                std::cout <<"upgrading";
                BWAPI::Broodwar->printf("Starting upgrade: %s", upgradeType.getName().c_str());
                return true;
            }
        }
    }

    return false;
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

    return true;
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
    const int supplyAvailable = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    const int unit_mineral = unitType.mineralPrice();
    const int unit_gas = unitType.gasPrice();
    const int supply = unitType.supplyRequired();

    //if (supply <= supplyAvailable) {
    //    return false;
    //}
    
    if (building && !building->isTraining() && unit_mineral <= excess_mineral && unit_gas <= excess_gas) {
        BWAPI::Broodwar->printf(
            "BaseSupervisor | Building %s | Started Training Unit %s"
            , building->getType().getName().c_str()
            , unitType.getName().c_str());

        bool successful = building->train(unitType);

        if (successful) {
            queuedJobs.removeTop();
        }
    }

    return true;
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
            if (buildingInstance->getType() == building.unitType) {
                if (building.status == BuildingStatus::UnderConstruction || building.status == BuildingStatus::OrderGiven) {
                    building.status = BuildingStatus::Constructed;
                    building.unit = buildingInstance;
                }
            }
        }
    }
}

void BaseSupervisor::assignIdleWorkes() {
    for (Building& building : buildings) {
        if (building.unitType == BWAPI::UnitTypes::Protoss_Nexus) {
            BWAPI::Unitset workersInRadius = building.unit->getUnitsInRadius(1000, BWAPI::Filter::IsWorker && BWAPI::Filter::IsIdle && BWAPI::Filter::IsOwned);
            for (BWAPI::Unit worker : workersInRadius) {
                if (!workers.contains(worker)) {

                   
                    workers.insert(worker);
                }
            }
        }
    }
    //Upgrade Purposes
    bool ExistCore=false;
    bool ExistCitadel = false;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core && unit->isCompleted()) {
            ExistCore= true;
        }
        if (unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun && unit->isCompleted()) {
            ExistCitadel= true;
        }
    }
    if (ExistCore) {
        JobBase singularityChargeUpgrade(
            6, // Priority
            ManagerType::ProductionManager, // Assigned Manager
            JobType::UpgradeJob, // Job Type
            false, // Blocking
            Importance::High // Importance
        );
        singularityChargeUpgrade.setUpgradeType(BWAPI::UpgradeTypes::Singularity_Charge);
        queuedJobs.queueItem(singularityChargeUpgrade);//?
        //queuedJobs.queueItem(JobBase JobBase(0,BaseSupervisor,JobType::UpgradeJob));
    }
    if (ExistCitadel) {
        JobBase legEnhancementsUpgrade(
            1, // Priority
            ManagerType::ProductionManager, // Assigned Manager
            JobType::UpgradeJob, // Job Type
            false, // Blocking
            Importance::Low // Importance, slightly lower than Singularity Charge
        );
        legEnhancementsUpgrade.setUpgradeType(BWAPI::UpgradeTypes::Leg_Enhancements);
        queuedJobs.queueItem(legEnhancementsUpgrade);
    }

}


void BaseSupervisor::assignWorkersToGas() {
    int totalWorkers = pDataResources->unitsAvailable.size();
    int desiredGasWorkers = totalWorkers / 3; // One third of the total workers
    int currentGasWorkers = 0;

    // First, count how many workers are currently gathering gas
    for (auto worker : workers) {
        Worker ws(worker);
        if (ws.status == WorkerStatus::GatheringGas) {
            currentGasWorkers++;
        }
    }

    // Now, assign workers to gas if we don't have enough
    if (currentGasWorkers < desiredGasWorkers && desiredGasWorkers <= totalWorkers) {
        for (const auto& building : buildings) {
            if (building.unitType == BWAPI::UnitTypes::Protoss_Assimilator && building.status == BuildingStatus::Constructed) {
                for (BWAPI::Unit worker : workers) {
                    // Find idle or mineral-gathering workers to reassign
                    Worker ws(worker);
                    if (ws.status == WorkerStatus::GatheringMineral || ws.status == WorkerStatus::Idle) {
                        if (rand() % 100 < 43) { // Assuming each Assimilator can have 3 workers
                            worker->rightClick(building.unit);
                            ws.status = WorkerStatus::GatheringGas;
                            currentGasWorkers++;
                            //break;
                        }
                    }
                    if (currentGasWorkers >= desiredGasWorkers) {
                        break; // We have assigned enough workers to gas
                    }
                }
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

