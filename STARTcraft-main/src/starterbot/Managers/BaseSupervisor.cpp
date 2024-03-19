#include "iostream"
#include "../Tools.h"
#include "../BT/Data.h"
#include "BaseSupervisor.h"
#include "../BT/BT.h"
#include "../Blackboard.h";
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
    upgradeEnhancements();
    researchProtossTechs();
    verifyAliveWorkers();
    verifyArePylonsNeeded();

    //additionalNexus();
    assignIdleWorkes(); // Assigns new idle workers to our list of available workers
    //if (buildnewnexus) { std::cout << "v workers" << std::endl; }
    assignWorkersToHarvest(); // Distributes available workers to either have gas/mineral harvest as default behaviour

    assignSquadProduction();

    if (buildnewnexus) newNexusInfo();
    // Updates data in the worker BT and calls the BT with `pBT->Evaluate(pDataResources)`
    pDataResources->unitsFarmingGas = gasMiners;
    pDataResources->unitsFarmingMinerals = mineralMiners;

    if (pBT != nullptr && pBT->Evaluate(pDataResources) != BT_NODE::RUNNING) {
        delete (BT_DECORATOR*)pBT;
        pBT = nullptr;
    }
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

bool BaseSupervisor::buildBuilding(const JobBase& job) {
    const BWAPI::UnitType b = job.getUnit();
    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gass = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = b.mineralPrice();
    const int unit_gas = b.gasPrice();

    // If we have the resource required to build building

    if (unit_mineral <= excess_mineral && unit_gas <= excess_gass) {
        const auto startedBuilding = buildBuilding(b); // Try to place building and order unit to build
        const BWAPI::TilePosition place = std::get<1>(startedBuilding); // Position of the building 
        
        const int started = std::get<0>(startedBuilding); // If a worker has started going to construction site

        // If worker is moving to construction site, allocate resources such that they are not used and update building status
        if (started == 1) {
            BWAPI::Broodwar->printf("Moving to Construct Building %s", b.getName().c_str());
            queuedBuildJobs.removeTop();

            Building building(place, b);
            building.status = BuildingStatus::OrderGiven;
            
            buildings.push_back(building);

            allocated_gas += b.gasPrice();
            allocated_minerals += b.mineralPrice();
            if (b == BWAPI::UnitTypes::Protoss_Nexus) {
                PotentialNexus = place;
                /******************************/
                buildnewnexus = true;
            }
            return true;
        }
    }

    return true;
}

bool BaseSupervisor::produceUnit(const JobBase& job) {
    const BWAPI::UnitType unitType = job.getUnit();

    // Count the buildings which are required to construct this unit
    for (auto const& x : unitType.requiredUnits()) {
        int countConstructed = countConstructedBuildingsofType(x.first);

        if (countConstructed < x.second) {
            return false;
        }
    }

    // Get the index of the building which we have in this base to produce this unit. If -1 we dont have this building.
    const std::unordered_set<int> buildingIdx = getProductionBuilding(unitType);

    const int excess_mineral = BWAPI::Broodwar->self()->minerals() - allocated_minerals;
    const int excess_gas = BWAPI::Broodwar->self()->gas() - allocated_gas;

    const int unit_mineral = unitType.mineralPrice();
    const int unit_gas = unitType.gasPrice();
    if (buildnewnexus) { std::cout << "2build"; }
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
                else {
                    //std::cout << "unsucessful training of"<< building->getType().getName().c_str()<< "Started Training" << unitType.getName().c_str()<<std::endl;
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
void BaseSupervisor::additionalNexus() {
    if (pDataResources->assimilatorAvailable) {
        const BWAPI::UnitType supplyProviderType = Tools::GetDepot()->getType();
        int desiredMineral = gameParser.baseParameters.nMineralMinersWanted - mineralMiners.size();
        int desiredGas = gameParser.baseParameters.nGasMinersWanted - gasMiners.size();
        if (supplyProviderType.gasPrice() < gasMiners.size() or supplyProviderType.mineralPrice() < mineralMiners.size()) return;

        JobBase job(0, ManagerType::BaseSupervisor, JobType::Building, false, Importance::High);
        job.setUnitType(supplyProviderType);
        job.setGasCost(supplyProviderType.gasPrice());
        job.setMineralCost(supplyProviderType.mineralPrice());

        queuedBuildJobs.queueTop(job);
        //const BWAPI::Unit nexus = Tools::GetDepot();
        ////const BWAPI::TilePosition p = nexus->getTilePosition();
        //const BWAPI::UnitType unit = nexus->getType();

        //const auto startedBuilding = buildBuilding(unit); // Try to place building and order unit to build
        //const BWAPI::TilePosition p = std::get<1>(startedBuilding); // Position of the building 
        ////started
        //Building building(p, unit);
        //building.unit = nexus;//
        //building.status = BuildingStatus::OrderGiven;

        //buildings.push_back(building);//
        //
        //std::cout << "add nex";
        //blackboard.baseNexuses.push_back(nexus);//
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

    // Replace this unit so we can meet resource demand 
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
    {
    for (Building& building : buildings) {
        if (building.unitType == BWAPI::UnitTypes::Protoss_Nexus) {
            if (buildnewnexus) { 
                int nearbyWorkers=3;

                // Convert TilePosition to Position (center of the tile)
                BWAPI::Position centerPos = BWAPI::Position(PotentialNexus) + BWAPI::Position(16, 16); // Each tile is 32x32 pixels

                // Get all units within the specified radius of the center position

                std::vector<BWAPI::Unit> unitsInRadius = Tools::GetThreeClosestWorkersToTilePosition(PotentialNexus, nearbyWorkers);// BWAPI::Broodwar->getUnitsInRadius(centerPos, 900);

                for (auto& unit : unitsInRadius) {
                    // Check if the unit is a worker
                    if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->isIdle()) {
                        std::cout << "u!";
                        //nearbyWorkers.push_back(unit);
                        if (!workers.contains(unit)) {
                            workers.insert(unit);

                            if (blackboard.scouts.contains(unit)) {
                                blackboard.scouts.erase(unit);
                            }
                        }
                    }
                }

            }
            else {
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
            //if (buildnewnexus)std::cout << "nex rad2";
        }
    }}
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
    if (b == BWAPI::UnitTypes::Protoss_Nexus) {
        BWAPI::TilePosition startLocation = BWAPI::Broodwar->self()->getStartLocation();
        int searchRadius = 2900; // adjustable

        for (int x = startLocation.x - searchRadius; x <= startLocation.x + searchRadius; ++x) {
            for (int y = startLocation.y - searchRadius; y <= startLocation.y + searchRadius; ++y) {
                BWAPI::TilePosition potentialLocation(x, y);

                // position within map and buildable
                bool canPlaceNexus = BWAPI::Broodwar->isBuildable(potentialLocation, true) and BWAPI::Broodwar->canBuildHere(potentialLocation, BWAPI::UnitTypes::Protoss_Nexus, nullptr, true);

                if (!canPlaceNexus) continue;
                if (nonNexusPlaces.contains(potentialLocation)) continue;
                // make sure it is close enough to the minerals
                bool closeToMinerals = false;
                for (const auto& mineral : BWAPI::Broodwar->getMinerals()) {
                    if (potentialLocation.getDistance(mineral->getTilePosition()) <= 114) { // adjustable
                        closeToMinerals = true;
                        break;
                    }
                }
                if (!closeToMinerals) continue;

                // whether away from other buildings
                bool tooCloseToOtherBuildings = false;
                for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
                    if (unit->getType().isBuilding() && potentialLocation.getDistance(unit->getTilePosition()) < minDistanceFromOtherBuildings) {
                        tooCloseToOtherBuildings = true;//minDistanceFromOtherBuildings=8.0
                        break;
                    }
                }
                if (tooCloseToOtherBuildings) { nonNexusPlaces.insert(potentialLocation); continue; }

                // Check distance from enemies
                bool tooCloseToEnemies = false;
                for (const auto& enemyUnit : BWAPI::Broodwar->enemy()->getUnits()) {
                    // Calculate the distance from the potential Nexus location to this enemy unit
                    double distanceToEnemy = BWAPI::Position(potentialLocation).getDistance(enemyUnit->getPosition());

                    if (distanceToEnemy < minDistanceFromEnemies) {
                        tooCloseToEnemies = true;
                        break; // No need to check other enemy units if one is already too close
                    }
                }

                if (tooCloseToEnemies) {
                    nonNexusPlaces.insert(potentialLocation); // Optional: keep track of locations that are too close to enemies
                    continue; // Skip this location and check the next one
                }


                // if all satisfies, it is suitable

                ////    BWAPI::TilePosition buildnexPos = BWAPI::Broodwar->getBuildLocation(b, expansionPosition, maxBuildRange, buildingOnCreep);
                ////    if (canPlaceNexus) std::cout << "New Nexus wkr"<<workers.size()<<std::endl;
                bool orderGivenNexus = builder->build(b, potentialLocation);
                ////    if (canPlaceNexus) { std::cout << "is?"; buildnewnexus = true; }
                if (orderGivenNexus) {
                        {  buildnewnexus = true; }
                        return std::make_tuple(1, potentialLocation);
                    }
                    else 
                   {
                    nonNexusPlaces.insert(potentialLocation);
                        return std::make_tuple(0, potentialLocation);
                    }
            }
        }

    }
    
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

int BaseSupervisor::countConstructedBuildingsofType(BWAPI::UnitType u) {
    int count = 0;

    for (Building& building : buildings) {
        if (building.status == BuildingStatus::Constructed && building.unitType == u) {
            count++;
        }
    }

    return count;
}

void BaseSupervisor::newNexusInfo() {
    BWAPI::Unit newNexus = nullptr;
    //for (int x = startLocation.x - 50; x <= startLocation.x + 50; ++x) {
    //    for (int y = startLocation.y - 50; y <= startLocation.y + 50; ++y) {
    //    }
    //}
    newNexus = Tools::GetNexusAtTilePosition(PotentialNexus); //Tools::GetClosestUnitTo(place, b);
    if (!newNexus) {
        if (buildnewnexus) {
            std::cout << "no new"; if(firstfail)VerifyNexus();
    } return; }

    //std::cout << "iter"; //building.unit = newNexus;
    //const BWAPI::Unit nexus = building.unit;// Tools::GetDepot();
    //const BWAPI::TilePosition p = nexus->getTilePosition();
    //const BWAPI::UnitType unit = nexus->getType();
    pDataResources->nexus = newNexus;
    blackboard.baseNexuses.push_back(newNexus);
    BWAPI::Unit mineral = Tools::GetClosestUnitTo(newNexus, BWAPI::Broodwar->getMinerals());
    const BWAPI::Position mineralPosition = mineral->getPosition();
    const BWAPI::Position nexusPosition = newNexus->getPosition();

    const int defencePosX = 3 * (nexusPosition.x - mineralPosition.x) + mineralPosition.x;
    const int defencePosY = 3 * (nexusPosition.y - mineralPosition.y) + mineralPosition.y;

    const BWAPI::Position defencePos(defencePosX, defencePosY);
    std::cout << "assign";
    baseChokepoint = defencePos;
    NexusSucess = true;
    BWAPI::Broodwar->drawTextScreen(defencePos, "Base Chokepoint: Defend\n");
}