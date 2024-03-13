#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include <tuple>
#include "Tools.h"
#include <set>

#include "Data/Building.h"
#include "Data/ListBuilding.h"
#include "Data/JobPriorityList.h"
#include "Data/Worker.h"
#include "DataResources.h"
#include "BT.h";

class BaseSupervisor : virtual ManagerBase {
public:
    // Constructor
    BaseSupervisor() noexcept : ManagerBase(ManagerType::BaseSupervisor) {
        const BWAPI::Unit nexus = Tools::GetDepot();
        const BWAPI::TilePosition p = nexus->getTilePosition();
        const BWAPI::UnitType unit = nexus->getType();

        Building building(p, unit);
        building.unit = nexus;
        building.status = BuildingStatus::Constructed;

        buildings.push_back(building);

        // Define the data for resource gathering BT

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
    
    // Functions
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

    // Set worker units which belong to base
    void addWorker(BWAPI::Unit newWorker) {
        workers.insert(newWorker);
    };

private:
    // Fields
    std::unordered_set<BWAPI::Unit> workers;

    float defence_dps = 0; // Damage Per Second our defence can provide
    std::vector<Building> buildings;

    JobPriorityList queuedJobs;

    int allocated_minerals = 0;
    int allocated_gas = 0;

    // Behaviour Trees
    BT_NODE* pBT;

    // Behaviour Tree Data
    DataResources* pDataResources;

    // Functions
    bool buildBuilding(const JobBase& job);
    bool produceUnit(const JobBase& job);
    void verifyActiveBuilds();
    void verifyFinishedBuilds();
    void assignIdleWorkes();

    // Helper methods
    std::tuple<int, BWAPI::TilePosition> buildBuilding(BWAPI::UnitType b);
    int getProductionBuilding(BWAPI::UnitType u);
    //void expandSupply();
    
};
