#pragma once

#include <optional>
#include "iostream"
#include "../BT/Data.h"
#include "ManagerBase.h"
#include <tuple>
#include "../Tools.h"
#include <set>

#include "../../starterbot/GameFileParser.hpp"
#include "Data/Building.h"
#include "Data/JobPriorityList.h"
#include "../BT/DataResources.h"
#include "../BT/BT.h";
#include "../Blackboard.h";


class BaseSupervisor : virtual ManagerBase {
public:
    // Constructor
    BaseSupervisor(Blackboard& blackboard, bool notfirst=false) noexcept : ManagerBase(ManagerType::BaseSupervisor, blackboard) {
        /* 
        * When we construct the baseSupervisor we just look for a Nexus we own and
        * add this to the buildings list using the Building.h wrapper such that we
        * can produce units for it
        * 
        * TDOD :: Change this such that it doesn't get some random depot - it was to work when
        *         we extend to new bases.
        */
        
        /*
        * Here we define the default behavior for workers as a BT, which is to collect
        * resources. Namely minerals and gas (gas if we have an assimilator)
        */
        gameParser.parse_game_file("../../src/starterbot/BotParameters/GameFile.json");

        pDataResources = new DataResources();
        pDataResources->nWantedWorkersFarmingMinerals = gameParser.baseParameters.nMineralMinersWanted;
        pDataResources->nWantedWorkersFarmingGas = gameParser.baseParameters.nGasMinersWanted;
        pDataResources->assimilatorAvailable = false;
        if (!notfirst) {
            const BWAPI::Unit nexus = Tools::GetDepot();
            const BWAPI::TilePosition p = nexus->getTilePosition();
            const BWAPI::UnitType unit = nexus->getType();

            Building building(p, unit);
            building.unit = nexus;
            //building.
            building.status = BuildingStatus::Constructed;

            buildings.push_back(building);
            pDataResources->nexus = nexus;

            blackboard.baseNexuses.push_back(nexus);
            const BWAPI::Unit mineral = Tools::GetClosestUnitTo(nexus, BWAPI::Broodwar->getMinerals());
            const BWAPI::Position mineralPosition = mineral->getPosition();
            const BWAPI::Position nexusPosition = nexus->getPosition();

            const int defencePosX = 3 * (nexusPosition.x - mineralPosition.x) + mineralPosition.x;
            const int defencePosY = 3 * (nexusPosition.y - mineralPosition.y) + mineralPosition.y;

            const BWAPI::Position defencePos(defencePosX, defencePosY);
            baseChokepoint = defencePos;


            BWAPI::Broodwar->drawTextScreen(defencePos, "Base Chokepoint: Defend\n");
        }
        else { buildnewnexus = false; }

        // Add base chokepoint as a defensive position
        

        // Define behaviour tree for resource gathering
        pBT = new BT_DECORATOR("EntryPoint", nullptr);

        //Farming gas and minerals forever - as workers' default behaviour
        BT_PARALLEL_SEQUENCER* pParallelSeq = new BT_PARALLEL_SEQUENCER("MainParallelSequence", pBT, 10);
        BT_DECO_REPEATER* pFarmingResourcesForeverRepeater = new BT_DECO_REPEATER("RepeatForeverFarmingResources", pParallelSeq, 0, true, false, false);
        BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES* pSendWorkerToResources = new BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES("SendWorkerToResources", pFarmingResourcesForeverRepeater);
    };
    
    // Called by the parent's on frame call, such that all managers have this onFrame
    void onFrame();
    bool nextnexus_ready() {
        return pDataResources->assimilatorAvailable;
    };
    void setnexus_loc(BWAPI::Position nexusPos) {
        //const BWAPI::Position nexusPos
    };
    // Used by parent managers to give this manager a new job
    void postJob(JobBase& job) { 
        if (job.importance == Importance::High) {
            std::cout
                << "BaseSupervisor | Prority HIGH | Got new job: "
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
                << "BaseSupervisor | Prority LOW | Got new job: "
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
    void deleteWorker(BWAPI::Unit newWorker) { workers.erase(newWorker); };
    bool containsWorker(BWAPI::Unit Worker) {
        return workers.contains(Worker);
    };
    bool VerifyNexus() {
        //std::cout << "nex num"<<blackboard.baseNexuses.size();
        //PotentialNexus = *PotLoc;
        const BWAPI::UnitType supplyProviderType = Tools::GetDepot()->getType();
        int desiredMineral = gameParser.baseParameters.nMineralMinersWanted - mineralMiners.size();
        int desiredGas = gameParser.baseParameters.nGasMinersWanted - gasMiners.size();
        if (supplyProviderType.gasPrice() < gasMiners.size() or supplyProviderType.mineralPrice() < mineralMiners.size()) return false;

        JobBase job(0, ManagerType::BaseSupervisor, JobType::Building, false, Importance::High);
        job.setUnitType(supplyProviderType);
        job.setGasCost(supplyProviderType.gasPrice());
        job.setMineralCost(supplyProviderType.mineralPrice());

        queuedBuildJobs.queueTop(job);
        return true;
    };
    // Add a new worker to the list of workers for this base
    void addWorker(BWAPI::Unit newWorker) {
        workers.insert(newWorker);
    };
    bool buildnewnexus=false;
    bool NexusSucess = false;

private:
    // Fields
    GameFileParser gameParser;

    BWAPI::Position baseChokepoint;

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

    // Worker default BT - collect resources
    BT_NODE* pBT;
    DataResources* pDataResources; // BT data struct - updated with this supervisor's available workers

    // Functions
    bool buildBuilding(const JobBase& job); // Builds a building if possible given a build job
    bool produceUnit(const JobBase& job); // Produces a unit if possible given a produce job
    void verifyActiveBuilds(); // If construction has started we can free up the allocated resources
    void verifyFinishedBuilds(); // Looks at the buildings list and checks if building is finished and thus update status
    void verifyAliveWorkers(); // If a worker has died, then we want to remove it from being accessible.
    void upgradeEnhancements();//UpGrades
    void researchProtossTechs();//research
    void verifyArePylonsNeeded();
    void additionalNexus();
    void assignIdleWorkes(); // Any new idle worker spawned by nexus is added to the available workers vector
    void assignWorkersToHarvest(); // Assigns workers to either mineral or gas collection as default behaviour
    void assignSquadProduction(); // Checks if we can raise a squad and if one is wanted
    
    // Helper methods
    std::tuple<int, BWAPI::TilePosition> buildBuilding(BWAPI::UnitType b); // Returns an int (0 - impossible, 1 - possible) and a position we build it on
    //int getProductionBuilding(BWAPI::UnitType u);  // Gets the index in 'buildings' which can produce the given unit. (if returns -1 then we can produce unit)
    
    std::set<BWAPI::UpgradeType> protossUpgrades = {
    BWAPI::UpgradeTypes::Singularity_Charge, // Dragoon Range Upgrade
    BWAPI::UpgradeTypes::Leg_Enhancements, // Zealot Speed Upgrade
    BWAPI::UpgradeTypes::Protoss_Plasma_Shields, // Shield Upgrade
    BWAPI::UpgradeTypes::Protoss_Ground_Weapons, // Ground Weapons Upgrade
    BWAPI::UpgradeTypes::Protoss_Ground_Armor, // Ground Armor Upgrade
    BWAPI::UpgradeTypes::Protoss_Air_Weapons, // Air Weapons Upgrade
    BWAPI::UpgradeTypes::Protoss_Air_Armor, // Air Armor Upgrade
    // Add other Protoss upgrades as needed
    BWAPI::UpgradeTypes::Scarab_Damage,
    BWAPI::UpgradeTypes::Reaver_Capacity,
    BWAPI::UpgradeTypes::Gravitic_Drive,
    BWAPI::UpgradeTypes::Sensor_Array,
    BWAPI::UpgradeTypes::Gravitic_Boosters,
    BWAPI::UpgradeTypes::Khaydarin_Amulet,
    BWAPI::UpgradeTypes::Apial_Sensors,
    BWAPI::UpgradeTypes::Gravitic_Thrusters,
    BWAPI::UpgradeTypes::Carrier_Capacity,
    BWAPI::UpgradeTypes::Khaydarin_Core,
    BWAPI::UpgradeTypes::Argus_Jewel,
    BWAPI::UpgradeTypes::Argus_Talisman
    };

    std::set<BWAPI::TechType> protossTechs = {
    BWAPI::TechTypes::Psionic_Storm,
    BWAPI::TechTypes::Hallucination,
    BWAPI::TechTypes::Recall,
    BWAPI::TechTypes::Stasis_Field,
    BWAPI::TechTypes::Disruption_Web,
    BWAPI::TechTypes::Mind_Control,
    BWAPI::TechTypes::Maelstrom,

    BWAPI::TechTypes::Feedback,
    BWAPI::TechTypes::Dark_Archon_Meld,
    BWAPI::TechTypes::Archon_Warp
    // Add other Protoss techs as needed
    };
    float minDistanceFromOtherBuildings = 15.0;
    double minDistanceFromEnemies = 1010;
    BWAPI::TilePosition PotentialNexus;
    bool firstfail =true;
    std::set<BWAPI::TilePosition> nonNexusPlaces;
    void newNexusInfo();
    std::unordered_set<int> getProductionBuilding(BWAPI::UnitType u);  // Gets the index in 'buildings' which can produce the given unit. (if returns -1 then we can produce unit)
    int countConstructedBuildingsofType(BWAPI::UnitType u); // Counts the number of constructed buildings we have which for a given type
};
