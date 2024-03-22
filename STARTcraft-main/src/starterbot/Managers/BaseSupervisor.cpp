#include "iostream"
#include "Tools.h"
#include <Data.h>
#include "BaseSupervisor.h"
#include "BT.h"

void BaseSupervisor::onFrame() {
    // Build queued buildings
    if (!queuedBuildJobs.isEmpty()) {
        JobBase& job = queuedBuildJobs.getTop();
        bool const skip = buildBuilding(job);

        // If job is impossible to fulfill outside of resource requirements, queue at bottom
        if (skip) {
            queuedBuildJobs.removeTop();
            queuedBuildJobs.queueBottom(job);
        }
    }

    // Produce queued units
    if (!queuedProductionJobs.isEmpty()) {
        JobBase& job = queuedProductionJobs.getTop();
        bool const skip = produceUnit(job);

        // If job is impossible to fulfill outside of resource requirements, queue at bottom
        if (skip) {
            queuedProductionJobs.removeTop();
            queuedProductionJobs.queueBottom(job);
        }
    }

    // Verifies statuses of various data structures which the bot relies on - see header for more details
    verifyActiveBuilds();
    verifyFinishedBuilds();
    verifyAliveWorkers();
    verifyArePylonsNeeded();
    verifyObserverScouts();
    //verifyDestroyedBuildings();

    assignIdleWorkes(); // Assigns new idle workers to our list of available workers
    assignWorkersToHarvest(); // Distributes available workers to either have gas/mineral harvest as default behaviour
    assignSquadProduction();
    // Update tech and general enhancements for units
    upgradeEnhancements();
    researchProtossTechs();

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

            return false;
        }
        else if (started == 0) {
            return true;
        }
    }

    return false;
}

bool BaseSupervisor::produceUnit(const JobBase& job) {
    const BWAPI::UnitType unitType = job.getUnit();

    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gas = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = unitType.mineralPrice();
    const int unit_gas = unitType.gasPrice();

    if (unit_mineral <= excess_mineral && unit_gas <= excess_gas) {
        // Count the buildings which are required to construct this unit
        for (auto const& x : unitType.requiredUnits()) {
            int countConstructed = countConstructedBuildingsofType(x.first);

            if (countConstructed < x.second) {
                //print("JOB FAILED - PRODUCTION"
                //    , "We do not have enough of: "
                //    + x.first.getName()
                //    + ", for unit:"
                //    + unitType.getName());

                return true;
            }
        }

        // Get the index of the building which we have in this base to produce this unit. If -1 we dont have this building.
        const std::unordered_set<int> buildingIdx = getProductionBuilding(unitType);

        for (const int i : buildingIdx) {
            const BWAPI::Unit building = buildings.at(i).unit;

            if (building && !building->isTraining()) {
                bool successful = building->train(unitType);

                if (successful) {
                    //print("JOB FUFILLED - PRODUCTION"
                    //    , "Producing in building: "
                    //    + building->getType().getName()
                    //    + ", of unit:"
                    //    + unitType.getName());

                    queuedProductionJobs.removeTop();
                    return false;
                }
            }
        }
    }

    return false;
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

                    // If the building produces soldiers then set the rally to be at the chokepoint
                    if (blackboard.barrackTypes.contains(building.unitType)) {
                        building.unit->setRallyPoint(baseChokepoint);
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

    // Perform the necessary accounting when this unit is dead, either it was a mineral worker or gas worker
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

        print("WORKER UNASSIGNED"
            , "A worker has been unassigned form the base!");
    }

    // Replace this unit so we can meet resource demand - posts a new production job
    int desiredMineral = gameParser.baseParameters.nMineralMinersWanted - mineralMiners.size();
    int desiredGas = gameParser.baseParameters.nGasMinersWanted - gasMiners.size();

    if (desiredMineral > 0 || desiredGas > 0) {
        BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
        int inProduction = queuedProductionJobs.countUnitTypes(workerType);
        int toProduce = (desiredMineral + desiredGas) - inProduction;

        if (toProduce > 0) {
            for (int i = 0; i < toProduce; i++) {
                JobBase replaceWorkerJob(0, ManagerType::BaseSupervisor, JobType::UnitProduction, false, Importance::High);
                replaceWorkerJob.setUnitType(workerType);

                queuedProductionJobs.queueTop(replaceWorkerJob);
            }
        }
    }
}

void BaseSupervisor::verifyArePylonsNeeded() {
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();
    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply > 4) // We use 4 such that we wont have issues with more late game units (which require more supply)
        return;

    if (queuedBuildJobs.presentInTopNPositions(BWAPI::UnitTypes::Protoss_Pylon, 1000))
        return;

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    JobBase job(0, ManagerType::BaseSupervisor, JobType::Building, false, Importance::High);
    job.setUnitType(supplyProviderType);
    job.setGasCost(supplyProviderType.gasPrice());
    job.setMineralCost(supplyProviderType.mineralPrice());

    queuedBuildJobs.queueTop(job);
    queuedBuildJobs.queueTop(job);
}

void BaseSupervisor::verifyObserverScouts() {
    const BWAPI::UnitType observerType = BWAPI::UnitTypes::Protoss_Observer;

    // Do we have the facilities to construct an observer such that we can scout with it?
    for (auto const& x : observerType.requiredUnits()) {
        int countConstructed = countConstructedBuildingsofType(x.first);

        if (countConstructed < x.second) {
            return;
        }
    }

    // Do we already have a production job waiting for an observer?
    if (queuedProductionJobs.presentInTopNPositions(observerType, 1000)) {
        return;
    }

    // Check if we already have observer units alive - we want 2 max such that we can hotswap them
    for (BWAPI::Unit const unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == observerType)
            return;
    }

    JobBase job(0, ManagerType::BaseSupervisor, JobType::UnitProduction, false, Importance::High);
    job.setUnitType(observerType);

    print("NO OBSERVER UNITS"
        , "We currently have no observers - adding production job");

    queuedProductionJobs.queueTop(job);
}

void BaseSupervisor::verifyDestroyedBuildings() {
    // Add the position indecies of the buildings which do not exist anymore
    std::unordered_set<int> buildingsIndxDestroyed;

    for (int i = 0; i < buildings.size(); i++) {
        if (buildings[i].status == BuildingStatus::Constructed) {
            if (!buildings[i].unit->exists()) {
                buildingsIndxDestroyed.insert(i);
            }
        }
    }

    // Remove these buildings and create new construction jobs such that they are replaced
    for (int i : buildingsIndxDestroyed) {
        if (buildings[i].unitType == BWAPI::UnitTypes::Protoss_Assimilator) {
            pDataResources->assimilatorAvailable = false;
            pDataResources->assimilatorUnit = nullptr;
        }

        buildings.erase(buildings.begin() + i);

        //print("BUILDING DESTROYED"
        //    , "We have lost a building of type: "
        //    + buildings[i].unitType.getName()
        //    + ", adding this back to the build queue");
    }
}

void BaseSupervisor::assignIdleWorkes() {
    // Get all new workers which are idle around our Nexus and add them to our list of available workers, which handles the
    // case where we produce new workers and have to update this list.
    BWAPI::Unitset workersInRadius = pDataResources->nexus->getUnitsInRadius(1000, BWAPI::Filter::IsWorker && BWAPI::Filter::IsIdle && BWAPI::Filter::IsOwned);
    for (BWAPI::Unit worker : workersInRadius) {
        if (!workers.contains(worker)) {
            workers.insert(worker);

            if (blackboard.scouts.contains(worker)) {
                blackboard.scouts.erase(worker);
            }
        }
    }
}

void BaseSupervisor::assignWorkersToHarvest() {
    // Based on the number of mineral workers and gas workers we want, assign the workers.
    // Priority is fill gas miners -> fill mineral miners. As there are fewer gas miners needed.
    if (mineralMiners.size() == gameParser.baseParameters.nMineralMinersWanted
        && gasMiners.size() == gameParser.baseParameters.nGasMinersWanted) {
        return;
    }

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
        else if (mineralMiners.size() < gameParser.baseParameters.nMineralMinersWanted
            && worker->isIdle()
            && !mineralMiners.contains(worker)) {

            mineralMiners.insert(worker);
            continue;
        }
    }
}

void BaseSupervisor::assignSquadProduction() {
    // Looks at squads which have been ordered to be produced on the blackboard, and creates production jobs for them.
    for (SquadProductionOrder& order : blackboard.squadProductionOrders) {
        if (!order.isConstructed) {
            for (UnitProductionOrder& unitOrder : order.productionOrder) {
                const std::unordered_set<int> buildingIndx = getProductionBuilding(unitOrder.unitType);
                bool canProduce = (!buildingIndx.empty()) && (unitOrder.jobsCount < unitOrder.orderCount);

                if (canProduce) {
                    JobBase produceAttacker(0, ManagerType::BaseSupervisor, JobType::UnitProduction, false, Importance::High);
                    produceAttacker.setUnitType(unitOrder.unitType);

                    queuedProductionJobs.queueBottom(produceAttacker);
                    unitOrder.jobsCount++;
                }
            }
        }
    }
}

// Helper methods ============================================================================


std::tuple<int, BWAPI::TilePosition> BaseSupervisor::buildBuilding(BWAPI::UnitType b) {
    // We try to build a building around a nexus or a pylon - this can cause framerates to drop sometimes, but this is
    // a quick fix to decrease the likelihood of buildings being unable to be placed.

    for (Building building : buildings) {
        if (building.status == BuildingStatus::Constructed) {
            if (building.unitType == BWAPI::UnitTypes::Protoss_Pylon || building.unitType == BWAPI::UnitTypes::Protoss_Nexus) {
                BWAPI::TilePosition desiredPos = building.unit->getTilePosition();
                const BWAPI::UnitType builderType = b.whatBuilds().first;

                BWAPI::Unit builder = findOptimalWorkerToBuild();

                if (!builder) { return std::make_tuple(-1, desiredPos); }

                // Ask BWAPI for a building location near the desired position for the type
                constexpr int maxBuildRange = 128;
                const bool buildingOnCreep = b.requiresCreep();
                BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(b, desiredPos, maxBuildRange, buildingOnCreep);

                bool orderGiven = builder->build(b, buildPos);
                int tries = 0;

                while (!orderGiven && tries < maxBuildPlacementTries) {
                    buildPos = BWAPI::Broodwar->getBuildLocation(b, desiredPos, maxBuildRange, buildingOnCreep);
                    orderGiven = builder->build(b, buildPos);

                    tries++;
                }

                if (orderGiven) {
                    print("BUILD ORDER GIVEN"
                        , "Building of type: "
                        + b.getName()
                        + " will now be built soon");

                    return std::make_tuple(1, buildPos);
                }

                else {
                    print("BUILD ORDER FAILED"
                        , "We could not find a placement for building: "
                        + b.getName());

                    return std::make_tuple(0, BWAPI::Broodwar->self()->getStartLocation());
                }
            }
        }
    }
}

std::unordered_set<int> BaseSupervisor::getProductionBuilding(BWAPI::UnitType u) {
    // Find the index of the building which can produce this unit type
    std::unordered_set<int> buildingIndecies;

    for (int i = 0; i < buildings.size(); i++) {
        BWAPI::UnitType::set canProduce = buildings.at(i).unitType.buildsWhat();

        if (canProduce.contains(u) && buildings.at(i).unit) {
            buildingIndecies.insert(i);
        }
    }

    return buildingIndecies;
}

int BaseSupervisor::countConstructedBuildingsofType(BWAPI::UnitType u) {
    // Count the number of buildings we have of some given type
    int count = 0;

    for (Building& building : buildings) {
        if (building.status == BuildingStatus::Constructed && building.unitType == u) {
            count++;
        }
    }

    return count;
}

BWAPI::Unit BaseSupervisor::findOptimalWorkerToBuild() {
    // We prioritise finding an idle worker to build, if not then we get the first available one
    if (!workers.empty()) {
        for (BWAPI::Unit worker : workers) {
            if (worker->isIdle()) {
                return worker;
            }
        }
        return *workers.begin();
    }
    return nullptr;
}

void BaseSupervisor::upgradeEnhancements() {
    for (auto upgradeType : protossUpgrades) {
        // Check if we have the building that can perform the upgrade
        BWAPI::Unit upgradeBuilding = nullptr;// getBuildingForUpgrade(upgradeType);
        for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
            // Check if the unit is the correct type of building and if it can perform the desired upgrade
            if (unit->getType().upgradesWhat().contains(upgradeType) && unit->isCompleted()) {
                upgradeBuilding = unit;
            }
        }

        if (!upgradeBuilding || upgradeBuilding->isUpgrading()) {
            continue; // Skip if we don't have the building or it's already busy
        }

        // Check if we can afford the upgrade and if it's not already researched
        if (BWAPI::Broodwar->canUpgrade(upgradeType, upgradeBuilding) && !BWAPI::Broodwar->self()->getUpgradeLevel(upgradeType)) {
            upgradeBuilding->upgrade(upgradeType);
            BWAPI::Broodwar->printf("Upgrading %s", upgradeType.getName().c_str());
        }
    }
}

void BaseSupervisor::print(std::string order, std::string msg) {
    std::cout << "BaseSupervisor | " << order << " | " << msg << std::endl;
}

//NEW:: 
void BaseSupervisor::researchProtossTechs() {
    for (auto techType : protossTechs) {
        // Check if we have the building that can perform the research
        BWAPI::Unit researchBuilding = nullptr;// getBuildingForTech(techType);
        for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
            // Check if the unit is the correct type of building and if it can perform the desired research
            if (unit->getType().researchesWhat().contains(techType) && unit->isCompleted()) {
                researchBuilding = unit;
            }
        }

        if (!researchBuilding || researchBuilding->isResearching()) {
            continue; // Skip if we don't have the building or it's already busy
        }

        // Check if we can afford the research and if it's not already researched
        if (BWAPI::Broodwar->canResearch(techType, researchBuilding) && !BWAPI::Broodwar->self()->hasResearched(techType)) {
            researchBuilding->research(techType);
            BWAPI::Broodwar->printf("Researching %s", techType.getName().c_str());
        }
    }
}
