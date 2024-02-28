#include <queue>
#include <vector>

#pragma once

enum class ManagerType {
    GameCommander,
    ScoutManager,
    ArmyManager,
    ProductionManager,
    BuildManager,
    BaseSupervisor
};

class JobBase {
public:
    JobBase(int priority, ManagerType assignedManager);

    // Setters
    void setPriority(int priority);

    // Getters
    int getPriority() const;
    ManagerType getAssignedManager() const;

    
private:
    int priority;
    ManagerType assignedManager;
};

struct JobComparator {
    bool operator()(const JobBase& job1, const JobBase& job2) const {
        // Compare based on priority
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority(); // Higher priority should come first
        }
        // If priorities are equal, compare by assigned manager
        return job1.getAssignedManager() > job2.getAssignedManager();
    }
};

using JobPriorityQueue = std::priority_queue<JobBase, std::vector<JobBase>, JobComparator>;