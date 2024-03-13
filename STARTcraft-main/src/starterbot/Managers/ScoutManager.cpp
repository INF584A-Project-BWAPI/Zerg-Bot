#include "ScoutManager.h"
#include "iostream"
#include "../BT/Data.h"
//#include <Data.h>

void ScoutManager::onFrame() {
    // Build queued buildings
    // looks at highest priority item without popping from queue yet
    if (!queuedJobs.isEmpty()) {
        const JobBase job = queuedJobs.getTop();

        std::cout << "We have a Scout job: " << job.getUnit() << std::endl;
    }
}


