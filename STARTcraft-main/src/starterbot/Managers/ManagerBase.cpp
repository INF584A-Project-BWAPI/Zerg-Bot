#include <iostream>
#include "ManagerBase.h"
#include "Data/JobBase.h"

bool ManagerBase::postJob(JobBase job) {
    queuedJobs.push(job);
    return true;
}

JobPriorityQueue ManagerBase::getManagerJobs(ManagerType assignedManager) {
    JobPriorityQueue managerQueue;
    JobPriorityQueue copyQueue = queuedJobs;

    while (!copyQueue.empty()) {
        const JobBase& job = copyQueue.top();

        if (job.getAssignedManager() == assignedManager) {
            managerQueue.push(job);
        }
 
        copyQueue.pop();
    }

    return managerQueue;
}
