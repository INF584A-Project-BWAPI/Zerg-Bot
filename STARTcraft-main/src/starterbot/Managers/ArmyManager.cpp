#include "ArmyManager.h"
#include "iostream"
#include "Data.h"

void ArmyManager::onFrame() {
    if (!queuedJobs.isEmpty()) {
        // Build queued buildings
        // looks at highest priority item without popping from queue yet
        JobBase job = queuedJobs.getTop();

        // Do something
        std::cout << "We have a Army job: " << job.getUnit() << std::endl;
    }
}

