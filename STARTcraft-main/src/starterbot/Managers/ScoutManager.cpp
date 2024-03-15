#include "ScoutManager.h"
#include "iostream"
#include "Tools.h"
#include <Data.h>

void ScoutManager::onFrame() {
    // Build queued buildings
    // looks at highest priority item without popping from queue yet

    // if I don't have any scouts, make one
    if (scouts.size() == 0) {
        const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
        for (BWAPI::Unit u : myUnits)
        {
            // Check the unit type, if it is an idle worker, then we want to make it a scout
            if (u->getType().isWorker())
            {
                ScoutManager::makeScout(u);
                break;
            }
        }
        // we'll also give them a job, move this elsewhere after prototyping
        JobBase j(0, ManagerType::ScoutManager, JobType::Scouting, false, Importance::Low);
        j.setTargetLocation(BWAPI::Broodwar->self()->getStartLocation());
        queuedJobs.queueBottom(j);
        // experiment with a couple more jobs -> leads to crash, ask patrick
        JobBase j2(0, ManagerType::ScoutManager, JobType::Scouting, false, Importance::Low);
        j2.setTargetLocation(BWAPI::Broodwar->self()->getStartLocation());
        queuedJobs.queueBottom(j2);
    }

    // iterate over active scouts, check on their jobs and state
    for (int idx = 0; idx < scouts.size(); ++idx) {
        checkOnScout( & scouts[idx]);
    }
    //for (scout s : scouts) {
    //    checkOnScout(s);
    //}

    // if you still have queued jobs, this means you need more scouts
    /*if (!queuedJobs.isEmpty()) {
        const JobBase job = queuedJobs.getTop();
        std::cout << "We have at least 1 unassigned Scout Job: " << job.getUnit() << std::endl;
        // find a mining worker -> make them a scout and assign the job to them
        // or just do nothing, and periodically increase the number of scouts you own
    }*/
}

void ScoutManager::makeScout(BWAPI::Unit u) {
    // make the scout struct and push it to the vector
    scout sc;
    sc.unit = u;
    scouts.push_back(sc);
    blackboard.scouts.insert(sc.unit);
}

/*void ScoutManager::unmakeScout(scout& s) {
    // find and remove that scout
    if (std::find(scouts.begin(), scouts.end(), s) != scouts.end()) {
        scouts.erase(std::find(scouts.begin(), scouts.end(), s));
    }
}*/ // fuck it, once a scout, forever a scout


void ScoutManager::checkOnScout(scout * s) {
    // does it have a job? -> check the bool
    if (!s->is_working()) { // no active job:
        //SendScouting if you have some queued Job, otherwise make "working" = false
        if (!queuedJobs.isEmpty()) {
            const JobBase job = queuedJobs.getTop();
            s->set_working(true);
            sendScouting(s, job);
            queuedJobs.removeTop(); // why does this crash?
        }
        //std::cout << "no more scouting jobs left!" << '\n';
    }
    else {
        // if it has a job, is it moving to it?
        //    -> what is it seeing? implement the churchill behavior tree TODO
        //    -> if it is mining or idle, that means it finished the job and try to find a job to assign to it, otherwise make "working" = false
        BWAPI::Unit u = s->unit;
        if (u->isGatheringMinerals() || u->isIdle()) { // isGatheringMinerals() is not a sufficient condition to check idleness
            std::cout << "this unit is idle, I will send it scouting" << '\n';
            s->set_working(false);
            //checkOnScout(s);
        }
        if (u->isMoving()) {
            //std::cout << "this unit is moving" << '\n';
        }
    }
}

void ScoutManager::sendScouting(scout * s, JobBase job) {
    // assign a unassigned job to a scout, making them move to an unexplored location
    std::cout << "Sending scout to explore" << '\n';
    if (ExploredLocations >= StartLocations.size()) { ExploredLocations = 0; } // if everything has been explored once, explore again
    if (StartLocations[ExploredLocations] == HomeLocation) { ExploredLocations = ExploredLocations % StartLocations.size(); } // don't explore own home

    if (job.getTargetLocation() == HomeLocation) {
        // go to the location assigned by the job EXCEPT if that location is home (default)
        job.setTargetLocation(StartLocations[ExploredLocations]);
    }
    s->set_job(job);
    BWAPI::Unit u = s->unit;
    u->move((BWAPI::Position)job.getTargetLocation());
    ExploredLocations++;
}

/*void ScoutManager::updateScouts()
{ // iterate over each scout, check if their moving, check what they are seeing, update the BTdata with what they see
}*/