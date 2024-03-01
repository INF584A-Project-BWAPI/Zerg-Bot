#include "ArmyManager.h"
#include "iostream"
#include <Data.h>

void ArmyManager::onFrame() {
    // Build queued buildings
    // looks at highest priority item without popping from queue yet
    std::optional<JobBase> jobOptional = peekTopQueuedJob();

    if (jobOptional.has_value()) {
        const JobBase job = *jobOptional;

        // Do something
        std::cout << "We have a Army job: " << job.getUnit() << std::endl;
    }
}

void ArmyManager::postJob(JobBase job) {
    queuedJobs.push(job);
}
