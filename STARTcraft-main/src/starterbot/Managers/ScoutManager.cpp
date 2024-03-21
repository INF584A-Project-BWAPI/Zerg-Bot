#include "ScoutManager.h"
#include "iostream"
#include "Tools.h"
#include <Data.h>

void ScoutManager::onFrame() {
    // Build queued buildings
    // looks at highest priority item without popping from queue yet

    syncWithBlackboard();

    // if I don't have any scouts, make one
    if (scouts.size() == 0) {
        const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
        for (BWAPI::Unit u : myUnits)
        {
            // Check the unit type, if it is an idle observer, then we want to make it a scout
            // will need to fix this such that it finds a FREE worker instead of just grabbing the first one
            //if (u->getType().isWorker())
            if (u->getType() == BWAPI::UnitTypes::Protoss_Observer)
            {
                ScoutManager::makeScout(u);
                break;
            }
        }
        
        if (scouts.size() == 0) {
            print("Finding Scouts", "Failed to find an observer to become scout, grabbing a worker");
            for (BWAPI::Unit u : myUnits)
            {
                if (u->getType().isWorker())
                {
                    ScoutManager::makeScout(u);
                    break;
                }
            }
        }

        for (int n_bases = 0; n_bases < StartLocations.size(); n_bases++) {
            // create a job per base to explore - 1 (homebase)
            JobBase j(0, ManagerType::ScoutManager, JobType::Scouting, false, Importance::Low);
            j.setTargetLocation(BWAPI::Broodwar->self()->getStartLocation());
            queuedJobs.queueBottom(j);
        }
    }


    // iterate over active scouts, check on their jobs and state
    for (int idx = 0; idx < scouts.size(); ++idx) {
        checkOnScout( & scouts[idx]);
    }

}

void ScoutManager::makeScout(BWAPI::Unit u) {
    // make the scout struct and push it to the vector
    print("Finding Scouts", "Found a worker, making it a scout");
    scout sc;
    sc.unit = u;
    scouts.push_back(sc);
    blackboard.scouts.insert(sc.unit);
}

void ScoutManager::unmakeScout(scout s) {
    // find and remove that scout
    int found = -1;
    for (int sc_idx = 0; sc_idx < scouts.size(); sc_idx++) {
        if (s.unit = scouts[sc_idx].unit) { found = sc_idx; break; }
    }
    if (found != -1) {
        scouts.erase(scouts.begin() + found);
    }
}

void ScoutManager::syncWithBlackboard() {
    // blackboard to vector of scouts sync
    for (BWAPI::Unit bb_scout : blackboard.scouts) {
        bool bb_scout_in_sc = false;
        for (int sc_idx = 0; sc_idx < scouts.size(); sc_idx++) {
            scout sc_scout = scouts[sc_idx];
            if (bb_scout == sc_scout.unit) {
                bb_scout_in_sc = true;
            }
        }
        if (!bb_scout_in_sc) {
            scout sc;
            sc.unit = bb_scout;
            scouts.push_back(sc);
        }
    }

    // vector of scouts to blackboard sync
    for (int sc_idx = 0; sc_idx < scouts.size(); sc_idx++) {
        scout sc_scout = scouts[sc_idx];
        if (!blackboard.scouts.contains(sc_scout.unit)) {
            //blackboard.scouts.insert(sc_scout.unit); // add the scout to the blackboard
            unmakeScout(sc_scout); // unmake the scout
        }
        if (!sc_scout.unit->exists()) { 
            print("Scout Exploration", "SCOUT IS DEADED");
            unmakeScout(sc_scout);
            blackboard.scouts.erase(sc_scout.unit);
        }
    }
}

void ScoutManager::checkOnScout(scout * s) {
    // does it have a job? -> check the bool
    if (!s->unit->exists()) { unmakeScout(*s); return; }
    if (!s->is_working()) { // no active job:
        //SendScouting if you have some queued Job, otherwise make "working" = false
        if (!queuedJobs.isEmpty()) {
            const JobBase job = queuedJobs.getTop();
            s->set_working(true);
            sendScouting(s, job);
            queuedJobs.removeTop(); // why does this crash?
        }
    }
    else {
        // if it has a job, is it moving to it?
        //    -> what is it seeing? implement the churchill behavior tree TODO
        //    -> if it is mining or idle, that means it finished the job and try to find a job to assign to it, otherwise make "working" = false
        BWAPI::Unit u = s->unit;
        if (u->isGatheringMinerals() || u->isIdle()) { // isGatheringMinerals() is not a sufficient condition to check idleness
            print("Finding Scout", "This unit is idle, I will send it scouting");
            s->set_working(false);
        }
        if (u->isMoving()) {
            BWAPI::Unitset scout_info = u->getUnitsInRadius(9000, BWAPI::Filter::IsEnemy);
            if (blackboard.scout_info.empty()) {
                blackboard.scout_info.push_back(scout_info);
                s->max_saw = scout_info.size();
                print("Scout Exploration", "scout sees multiple baddies.");
                return;
            }
            if (scout_info.size() != 0 && scout_info.size() > blackboard.scout_info.back().size()) {
                blackboard.scout_info.push_back(scout_info);
                s->max_saw = scout_info.size();
                print("Scout Exploration", "scout sees multiple baddies.");
            }
            if (u->getHitPoints() < s->prev_hp) {
                print("Scout Exploration", "scout is hurt, rushing back home.");
                u->move((BWAPI::Position)HomeLocation);
            }
        }
    }
    s->prev_hp = s->unit->getHitPoints();
}

void ScoutManager::sendScouting(scout * s, JobBase job) {
    // assign a unassigned job to a scout, making them move to an unexplored location
    print("Scout Exploration", "Sending scout to explore");
    if (ExploredLocations >= StartLocations.size()) { ExploredLocations = 0; } // if everything has been explored once, explore again
    if (StartLocations[ExploredLocations] == HomeLocation) { ExploredLocations = ExploredLocations % StartLocations.size(); } // don't explore own home

    if (job.getTargetLocation() == HomeLocation) {
        // go to the location assigned by the job EXCEPT if that location is home (default)
        job.setTargetLocation(StartLocations[ExploredLocations]);
    }
    s->set_job(job);
    //s->max_saw = 0; // reset max saw because its a new exploring job
    BWAPI::Unit u = s->unit;
    u->move((BWAPI::Position)job.getTargetLocation());
    u->move((BWAPI::Position)HomeLocation, true);
    ExploredLocations++;
}

void ScoutManager::print(std::string order, std::string msg) {
    std::cout << "ScoutManager | " << order << " | " << msg << std::endl;
}
