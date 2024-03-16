#include "iostream"
#include "Tools.h"
#include <Data.h>
#include "BaseSupervisor.h"
#include "BT.h"

void BaseSupervisor::onFrame() {
    // Build queued buildings
    if (!queuedBuildJobs.isEmpty()) {
        const JobBase& job = queuedBuildJobs.getTop();
        buildBuilding(job);
    }

    if (!queuedProductionJobs.isEmpty()) {
        const JobBase& job = queuedProductionJobs.getTop();
        produceUnit(job);
    }

    // Verifies statuses of buildings and assigns new idle workers to this bases workers list
    verifyActiveBuilds();
    verifyFinishedBuilds();
    verifyAliveWorkers();
    verifyArePylonsNeeded();

    assignIdleWorkes(); // Assigns new idle workers to our list of available workers
    assignWorkersToHarvest(); // Distributes available workers to either have gas/mineral harvest as default behaviour
    assignSquadProduction();

    // Updates data in the worker BT and calls the BT with `pBT->Evaluate(pDataResources)`
    pDataResources->unitsFarmingGas = gasMiners;
    pDataResources->unitsFarmingMinerals = mineralMiners;

    if (pBT != nullptr && pBT->Evaluate(pDataResources) != BT_NODE::RUNNING) {
        delete (BT_DECORATOR*)pBT;
        pBT = nullptr;
    }
}


bool BaseSupervisor::buildBuilding(const JobBase& job) {
    const BWAPI::UnitType b = job.getUnit();
    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gass = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = b.mineralPrice();
    const int unit_gas = b.gasPrice();

    // If we have the resource required to build building
    if (unit_mineral <= excess_mineral && unit_gas <= excess_gass) {
        const auto startedBuilding = buildBuilding(b); // Try to place building and order unit to build
        const BWAPI::TilePosition p = std::get<1>(startedBuilding); // Position of the building 
        const int started = std::get<0>(startedBuilding); // If a worker has started going to construction site

        // If worker is moving to construction site, allocate resources such that they are not used and update building status
        if (started == 1) {
            BWAPI::Broodwar->printf("Moving to Construct Building %s", b.getName().c_str());
            queuedBuildJobs.removeTop();

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

    // Get the index of the building which we have in this base to produce this unit. If -1 we dont have this building.
    const std::unordered_set<int> buildingIdx = getProductionBuilding(unitType);

    if (buildingIdx.empty()) {
        BWAPI::Broodwar->printf("Could Not Find a Building to Produce %s", unitType.getName().c_str());
        return false;
    }

    for (int i : buildingIdx) {
        for (int j : buildingIdx) {
            const BWAPI::Unit buildingj = buildings.at(j).unit;
            const BWAPI::Unit buildingi = buildings.at(i).unit; 

            if (buildingj && buildingi && buildingi == buildingj && i != j) {
                BWAPI::Broodwar->printf(
                    "BaseSupervisor | ERROR | We have two buildings of same unit instance");
            }
        }
    }

    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gas = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = unitType.mineralPrice();
    const int unit_gas = unitType.gasPrice();

    if (unit_mineral <= excess_mineral && unit_gas <= excess_gas) {
        for (const int i : buildingIdx) {
            const BWAPI::Unit building = buildings.at(i).unit;

            if (building && !building->isTraining()) {
                bool successful = building->train(unitType);

                if (successful) {
                    BWAPI::Broodwar->printf(
                        "BaseSupervisor | Building %s | Started Training Unit %s"
                        , building->getType().getName().c_str()
                        , unitType.getName().c_str());

                    queuedProductionJobs.removeTop();
                    break;
                }
            }
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
            // OrderGiven means that a worker is on the way to construct the building.
            if (building.status == BuildingStatus::OrderGiven) {
                // Check that this building and the building instance from the previous for loop is the same through position.
                const float dx = building.position.x - buildingStarted->getTilePosition().x;
                const float dy = building.position.y - buildingStarted->getTilePosition().y;

                if (dx * dx + dy * dy == 0.0) {
                    // If they are the same then we know it is underConstruction and we can unlock the previously allocated resource.
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
    // When the building is finished constructing, we want to update its entry in our list with its instance and say it is constructed
    // Thus, we can use this building after this (for example to produce units).
    for (const BWAPI::Unit& buildingInstance : BWAPI::Broodwar->getAllUnits()) {
        if (!buildingInstance->getType().isBuilding() || !buildingInstance->isCompleted()) {
            continue;
        }

        for (Building& building : buildings) {
            if (buildingInstance->getType() == building.unitType && building.status == BuildingStatus::UnderConstruction) {
                const float dx = building.position.x - buildingInstance->getTilePosition().x;
                const float dy = building.position.y - buildingInstance->getTilePosition().y;

                if (dx * dx + dy * dy == 0.0) {
                    building.status = BuildingStatus::Constructed;
                    building.unit = buildingInstance;

                    // If the building is an assimilator then we need to update pDataResources such that workers can collect gas now
                    if (buildingInstance->getType() == BWAPI::UnitTypes::Protoss_Assimilator) {
                        pDataResources->assimilatorAvailable = true;
                        pDataResources->assimilatorUnit = buildingInstance;
                    }

                    break;
                }
            }
        }
    }
}

void BaseSupervisor::verifyAliveWorkers() {
    // Remove any workers which are no longer alive
    std::unordered_set<BWAPI::Unit> unitsToRemove;

    for (BWAPI::Unit worker : workers) {
        if (!worker->exists()) {
            if (gasMiners.contains(worker)) {
                gasMiners.erase(worker);
            }

            if (mineralMiners.contains(worker)) {
                mineralMiners.erase(worker);
            }

            unitsToRemove.insert(worker);
        }

        if (blackboard.scouts.contains(worker)) {
            if (gasMiners.contains(worker)) {
                gasMiners.erase(worker);
            }

            if (mineralMiners.contains(worker)) {
                mineralMiners.erase(worker);
            }

            unitsToRemove.insert(worker);
        }
    }

    for (BWAPI::Unit worker : unitsToRemove) {
        workers.erase(worker);
    }
}

void BaseSupervisor::verifyArePylonsNeeded() {
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();
    bool pylonPlanned = false;

    if (!queuedBuildJobs.isEmpty())
        pylonPlanned = queuedBuildJobs.getTop().getUnit() == BWAPI::UnitTypes::Protoss_Pylon;


    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply > 2 || pylonPlanned) {
        return;
    }

    // Otherwise, we are going to build a supply provider
    //BWAPI::Broodwar->printf("Supply is running out (building more): %s", unusedSupply);

    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    JobBase job(0, ManagerType::BaseSupervisor, JobType::Building, false, Importance::High);
    job.setUnitType(supplyProviderType);
    job.setGasCost(supplyProviderType.gasPrice());
    job.setMineralCost(supplyProviderType.mineralPrice());

    queuedBuildJobs.queueTop(job);
}

void BaseSupervisor::assignIdleWorkes() {
    // Get all new workers which are idle around our Nexus and add them to our list of available workers, which handles the
    // case where we produce new workers and have to update this list.
    for (Building& building : buildings) {
        if (building.unitType == BWAPI::UnitTypes::Protoss_Nexus) {
            BWAPI::Unitset workersInRadius = building.unit->getUnitsInRadius(1000, BWAPI::Filter::IsWorker && BWAPI::Filter::IsIdle && BWAPI::Filter::IsOwned);
            for (BWAPI::Unit worker : workersInRadius) {
                if (!workers.contains(worker)) {
                    workers.insert(worker);

                    if (blackboard.scouts.contains(worker)) {
                        blackboard.scouts.erase(worker);
                    }
                }
            }
        }
    }
}

void BaseSupervisor::assignWorkersToHarvest() {
    for (BWAPI::Unit worker : workers) {
        if (gasMiners.size() < gameParser.baseParameters.nGasMinersWanted && pDataResources->assimilatorAvailable) {
            if ((worker->isIdle() || (!worker->isCarryingMinerals() && worker->isGatheringMinerals())) && !gasMiners.contains(worker)) {
                if (mineralMiners.contains(worker)) {
                    mineralMiners.erase(worker);
                }

                gasMiners.insert(worker);
                continue;
            }
        }
        else if (mineralMiners.size() < NWANTED_WORKERS_FARMING_MINERALS 
            && worker->isIdle()
            && !mineralMiners.contains(worker)) {
            mineralMiners.insert(worker);
            continue;
        }
    }
}

void BaseSupervisor::assignSquadProduction() {
    for (SquadProductionOrder& order : blackboard.squadProductionOrders) {
        if (!order.isConstructed) {
            for (UnitProductionOrder& unitOrder : order.productionOrder) {
                const std::unordered_set<int> buildingIndx = getProductionBuilding(unitOrder.unitType);
                bool canProduce = (!buildingIndx.empty()) && (unitOrder.jobsCount < unitOrder.orderCount);

                if (canProduce) {
                    JobBase produceAttacker(0, ManagerType::BaseSupervisor, JobType::UnitProduction, false, Importance::High);
                    produceAttacker.setUnitType(unitOrder.unitType);

                    queuedProductionJobs.queueTop(produceAttacker);
                    unitOrder.jobsCount++;
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

    BWAPI::Unit builder = *workers.begin();

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

std::unordered_set<int> BaseSupervisor::getProductionBuilding(BWAPI::UnitType u) {
    std::unordered_set<int> buildingIndecies;

    for (int i = 0; i < buildings.size(); i++) {
        BWAPI::UnitType::set canProduce = buildings.at(i).unitType.buildsWhat();

        if (canProduce.contains(u) && buildings.at(i).unit) {
            buildingIndecies.insert(i);
        }
    }

    return buildingIndecies;
}

