#pragma once

#include <optional>
#include "iostream"
#include "../BT/Data.h"
#include "ManagerBase.h"
#include "Data/Building.h"
#include "Data/ListBuilding.h"
#include "Data/JobPriorityList.h"
#include <tuple>
#include "../Tools.h"

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

private:
    // Fields
    std::vector<BWAPI::Unit> workers;
    float defence_dps = 0; // Damage Per Second our defence can provide
    std::vector<Building> buildings;

    JobPriorityList queuedJobs;

    int allocated_minerals = 0;
    int allocated_gas = 0;

    // Functions
    bool buildBuilding(const JobBase& job);
    bool produceUnit(const JobBase& job);
    void verifyActiveBuilds();
    void verifyFinishedBuilds();

    // Helper methods
    std::tuple<int, BWAPI::TilePosition> buildBuilding(BWAPI::UnitType b);
    int getProductionBuilding(BWAPI::UnitType u);
    //void expandSupply();
    
};
