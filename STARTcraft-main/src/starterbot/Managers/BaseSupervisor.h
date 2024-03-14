#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include <tuple>
#include "Tools.h"
#include <set>

#include "Data/Building.h"
#include "Data/JobPriorityList.h"
#include "Data/Worker.h"
#include "DataResources.h"
#include "BT.h";

class BaseSupervisor : virtual ManagerBase {
public:
    // Constructor
    BaseSupervisor() noexcept : ManagerBase(ManagerType::BaseSupervisor) {
        /* 
        * When we construct the baseSupervisor we just look for a Nexus we own and
        * add this to the buildings list using the Building.h wrapper such that we
        * can produce units for it
        * 
        * TDOD :: Change this such that it doesn't get any depot - it was to work when
        *         we extend to new bases.
        */

        const BWAPI::Unit nexus = Tools::GetDepot();
        const BWAPI::TilePosition p = nexus->getTilePosition();
        const BWAPI::UnitType unit = nexus->getType();

        Building building(p, unit);
        building.unit = nexus;
        building.status = BuildingStatus::Constructed;

        buildings.push_back(building);

        /*
        * Here we define the default behavior for workers as a BT, which is to collect
        * resources. Namely minerals and gas (gas if we have an assimilator)
        * 
        * TODO :: Extand BT such that the workers also collect gas.
        */

        pDataResources = new DataResources();
        pDataResources->currMinerals = 0;
        pDataResources->thresholdMinerals = THRESHOLD1_MINERALS;

        pDataResources->nWantedWorkersTotal = NWANTED_WORKERS_TOTAL;
        pDataResources->nWantedWorkersFarmingMinerals = NWANTED_WORKERS_FARMING_MINERALS;

        // Define behaviour tree for resource gathering
        pBT = new BT_DECORATOR("EntryPoint", nullptr);

        BT_PARALLEL_SEQUENCER* pParallelSeq = new BT_PARALLEL_SEQUENCER("MainParallelSequence", pBT, 10);

        //Farming Minerals forever
        BT_DECO_REPEATER* pFarmingResourcesForeverRepeater = new BT_DECO_REPEATER("RepeatForeverFarmingResources", pParallelSeq, 0, true, false, false);
        BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES* pNotEnoughWorkersFarmingResources = new BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES("NotEnoughWorkersFarmingResources", pFarmingResourcesForeverRepeater);
        BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES* pSendWorkerToResources = new BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES("SendWorkerToResources", pNotEnoughWorkersFarmingResources);
    };
    
    // Called by the parent's on frame call, such that all managers have this onFrame
    void onFrame();

    // Used by parent managers to give this manager a new job
    void postJob(JobBase& job) { 
        if (job.importance == Importance::High) {
            std::cout
                << "BaseSupervisor | Prority HIGH | Got new job: "
                << job.getUnit().getName().c_str()
                << std::endl;
            queuedJobs.queueTop(job);
        }
        else if (job.importance == Importance::Low) {
            std::cout
                << "BaseSupervisor | Prority LOW | Got new job: "
                << job.getUnit().getName().c_str()
                << std::endl;
            queuedJobs.queueBottom(job);
        }
    };

    // Add a new worker to the list of workers for this base
    void addWorker(BWAPI::Unit newWorker) {
        workers.insert(newWorker);
    };

private:
    // Fields
    std::unordered_set<BWAPI::Unit> workers;

    float defence_dps = 0; // Damage Per Second our defence can provide
    std::vector<Building> buildings; // See Buildings.h - used to verify the construction status of buildings

    JobPriorityList queuedJobs;

    // Resource allocation when constructing such that we can produce units and build in parallel
    int allocated_minerals = 0;
    int allocated_gas = 0;

    // Worker default BT - collect resources
    BT_NODE* pBT;
    DataResources* pDataResources; // BT data struct - updated with this supervisor's available workers

    // Functions
    bool buildBuilding(const JobBase& job); // Builds a building if possible given a build job
    bool produceUnit(const JobBase& job); // Produces a unit if possible given a produce job
    void verifyActiveBuilds(); // If construction has started we can free up the allocated resources
    void verifyFinishedBuilds(); // Looks at the buildings list and checks if building is finished and thus update status
    void assignIdleWorkes(); // Any new idle worker spawned by nexus is added to the available workers vector

    // Helper methods
    std::tuple<int, BWAPI::TilePosition> buildBuilding(BWAPI::UnitType b); // Returns an int (0 - impossible, 1 - possible) and a position we build it on
    int getProductionBuilding(BWAPI::UnitType u);  // Gets the index in 'buildings' which can produce the given unit. (if returns -1 then we can produce unit)
};
