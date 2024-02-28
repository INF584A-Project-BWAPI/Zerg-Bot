#include "Data/JobBase.h"


class ManagerBase; // Forward declaration

class ManagerBase {
public:
    // Default constructor
    ManagerBase() : parent(nullptr) {}

    // Setter functions
    void setParent(ManagerBase* parent) {
        this->parent = parent;
    }

    void addChild(ManagerBase* child) {
        children.push_back(child);
    }

    // Getter for queuedJobs
    const JobPriorityQueue& getQueuedJobs() const {
        return queuedJobs;
    }

    void removeTopJob() {
        queuedJobs.pop();
    }

    // Getter for activeJobs
    const std::vector<JobBase>& getActiveJobs() const {
        return activeJobs;
    }

    bool postJob(JobBase job);
    JobPriorityQueue getManagerJobs(ManagerType assignedManager);

private:
    ManagerBase* parent;
    std::vector<ManagerBase*> children;

    JobPriorityQueue queuedJobs;
    std::vector<JobBase> activeJobs;
};