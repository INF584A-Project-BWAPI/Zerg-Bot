#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include <tuple>
#include "Tools.h"
#include <set>

#include "GameFileParser.hpp"
#include "Data/Building.h"
#include "Data/JobPriorityList.h"
#include "DataResources.h"
#include "BT.h"
#include "Blackboard.h"


class AuxBaseSupervisor : virtual ManagerBase {
public:
    // Constructor
    AuxBaseSupervisor(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::AuxBaseSupervisor, blackboard) {
        /*
        * When we construct the AuxBaseSupervisor we just look for a Nexus we own and
        * add this to the buildings list using the Building.h wrapper such that we
        * can produce units for it
        *
        * TDOD :: Change this such that it doesn't get some random depot - it was to work when
        *         we extend to new bases.
        */


            //gameParser.parse_game_file("../../src/starterbot/BotParameters/NewNexusFile.json");

            pDataResources = new DataResources();
            pDataResources->nWantedWorkersFarmingMinerals = 2; //gameParser.baseParameters.nMineralMinersWanted;
            pDataResources->nWantedWorkersFarmingGas = 1;// gameParser.baseParameters.nGasMinersWanted;
            pDataResources->assimilatorAvailable = false;
            buildNewNexus = true;





        // Define behaviour tree for resource gathering
        pBT = new BT_DECORATOR("EntryPoint", nullptr);

        //Farming gas and minerals forever - as workers' default behaviour
        BT_PARALLEL_SEQUENCER* pParallelSeq = new BT_PARALLEL_SEQUENCER("MainParallelSequence", pBT, 10);
        BT_DECO_REPEATER* pFarmingResourcesForeverRepeater = new BT_DECO_REPEATER("RepeatForeverFarmingResources", pParallelSeq, 0, true, false, false);
        BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES* pSendWorkerToResources = new BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES("SendWorkerToResources", pFarmingResourcesForeverRepeater);

            const BWAPI::UnitType supplyProviderType = Tools::GetDepot()->getType();

        //    int residualMineralMiners = gameParser.baseParameters.nMineralMinersWanted - mineralMiners.size();
        //    int residualGasMiners = gameParser.baseParameters.nGasMinersWanted - gasMiners.size();


            //JobBase job(0, ManagerType::AuxBaseSupervisor, JobType::Building, false, Importance::High);
            //job.setUnitType(supplyProviderType);
            //job.setGasCost(supplyProviderType.gasPrice());
            //job.setMineralCost(supplyProviderType.mineralPrice());

            //queuedJobs.push_back(job);
            std::cout << "new job of new nexus"<<std::endl;

    };

    // Called by the parent's on frame call, such that all managers have this onFrame
    void onFrame();

    // Used by parent managers to give this manager a new job
    void postJob(JobBase job) {
        //queuedJobs.push_back(job);
        //std::cout << "seq";
        postJobQueue(job);
    }
    void postJobQueue(JobBase &job){
        if (job.importance == Importance::High) {
            std::cout
                << "AuxBaseSupervisor | Prority HIGH | Got new job: "
                << job.getUnit().getName().c_str()
                << std::endl;

            if (job.getJobType() == JobType::Building) {
                queuedBuildJobs.queueTop(job);
            }
            else if (job.getJobType() == JobType::UnitProduction) {
                queuedProductionJobs.queueTop(job);
            }
        }
        else if (job.importance == Importance::Low) {
            std::cout
                << "AuxBaseSupervisor | Prority LOW | Got new job: "
                << job.getUnit().getName().c_str()
                << std::endl;

            if (job.getJobType() == JobType::Building) {
                queuedBuildJobs.queueBottom(job);
            }
            else if (job.getJobType() == JobType::UnitProduction) {
                queuedProductionJobs.queueBottom(job);
            }
        }
    };

    // Add a new worker to the list of workers for this base
    void addWorker(BWAPI::Unit newWorker) {
        workers.insert(newWorker);
    };
    bool containsWorker(BWAPI::Unit Worker) { return workers.contains(Worker); };
    //void deleteWorker(BWAPI::Unit worker) { workers.erase(worker); };
    //void verifyNexus() {
    //    const BWAPI::UnitType supplyProviderType = Tools::GetDepot()->getType();

    //    int residualMineralMiners = gameParser.baseParameters.nMineralMinersWanted - mineralMiners.size();
    //    int residualGasMiners = gameParser.baseParameters.nGasMinersWanted - gasMiners.size();


    //    JobBase job(0, ManagerType::AuxBaseSupervisor, JobType::Building, false, Importance::High);
    //    job.setUnitType(supplyProviderType);
    //    job.setGasCost(supplyProviderType.gasPrice());
    //    job.setMineralCost(supplyProviderType.mineralPrice());

    //    queuedBuildJobs.queueTop(job);


    //};
    bool buildNewNexus= true;
    

private:
    // Fields
    GameFileParser gameParser;
    std::vector<JobBase> queuedJobs;
    BWAPI::Position baseChokepoint;
    bool newNexusIsAssigned = false;
    std::unordered_set<BWAPI::Unit> workers;
    std::unordered_set<BWAPI::Unit> gasMiners;
    std::unordered_set<BWAPI::Unit> mineralMiners;

    float defence_dps = 0; // Damage Per Second our defence can provide
    std::vector<Building> buildings; // See Buildings.h - used to verify the construction status of buildings

    JobPriorityList queuedBuildJobs;
    JobPriorityList queuedProductionJobs;

    // Resource allocation when constructing such that we can produce units and build in parallel
    int allocated_minerals = 0;
    int allocated_gas = 0;

    int maxBuildPlacementTries = 0;

    // Worker default BT - collect resources
    BT_NODE* pBT;
    DataResources* pDataResources; // BT data struct - updated with this supervisor's available workers
    //NEW::
    BWAPI::TilePosition potentialNexus;

    // Parameters used to determine location for a new AuxBaseSupervisor
    float minDistanceFromOtherBuildings = 10.0;
    double minDistanceFromEnemies = 1010;

    // Functions
    bool buildBuilding(const JobBase& job); // Builds a building if possible given a build job
    bool produceUnit(const JobBase& job); // Produces a unit if possible given a produce job
    void verifyActiveBuilds(); // If construction has started we can free up the allocated resources
    void verifyFinishedBuilds(); // Looks at the buildings list and checks if building is finished and thus update status
    void verifyAliveWorkers(); // If a worker has died, then we want to remove it from being accessible.
    void verifyArePylonsNeeded(); // Check if we need new pylons to meet production demand.
    void verifyObserverScouts(); // Check if we have any Observer units for scouts if not then we need to produce some.
    void verifyDestroyedBuildings(); // Checks if any buildings we own have been destroyed or not.

    void assignIdleWorkes(); // Any new idle worker spawned by nexus is added to the available workers vector
    void assignWorkersToHarvest(); // Assigns workers to either mineral or gas collection as default behaviour
    void assignSquadProduction(); // Checks if we can raise a squad and if one is wanted

    // Helper methods
    std::tuple<int, BWAPI::TilePosition> buildBuilding(BWAPI::UnitType b); // Returns an int (0 - impossible, 1 - possible) and a position we build it on
    std::unordered_set<int> getProductionBuilding(BWAPI::UnitType u);  // Gets the index in 'buildings' which can produce the given unit. (if returns -1 then we can produce unit)
    int countConstructedBuildingsofType(BWAPI::UnitType u); // Counts the number of constructed buildings we have which for a given type
    BWAPI::Unit findOptimalWorkerToBuild(); // Finds preferably an idle worker and then finds a worker mining, then finds one building.
    // Update tech and general enhancements for units
    std::set<BWAPI::TilePosition> nonNexusPlaces;
    // Debugging
    void print(std::string order, std::string msg);
};
