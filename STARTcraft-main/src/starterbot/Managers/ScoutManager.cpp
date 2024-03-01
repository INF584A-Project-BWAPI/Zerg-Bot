#include "ScoutManager.h"
#include "iostream"
#include <Data.h>

void ScoutManager::onFrame() {
    // Build queued buildings
    // looks at highest priority item without popping from queue yet
    std::optional<JobBase> jobOptional = peekTopQueuedJob();

    if (jobOptional.has_value()) {
        const JobBase job = *jobOptional;

        // Do something
        std::cout << "We have a Scout job: " << job.getUnit() << std::endl;
    }
}

void ScoutManager::postJob(JobBase job) {
    queuedJobs.push(job);
}

