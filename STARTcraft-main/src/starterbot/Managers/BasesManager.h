#pragma once

#include <vector>
#include "Data/JobBase.h"
#include "ManagerBase.h"
#include "BaseSupervisor.h"
#include "BasesManager.h"

class BasesManager: public ManagerBase {
public:
    // Constructor
    BasesManager(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::BasesManager, blackboard) {};


    // Setters
    void setParent(ManagerBase* parent) noexcept { parent = parent; }
    void setChild(BaseSupervisor* child) noexcept { children.push_back(child); }

    // Getters
    const ManagerBase* getParent() const noexcept { return parent; };
    const std::vector<BaseSupervisor*> getChildren() const noexcept { return children; };

    // Functions
    void onFrame();
    void postJob(JobBase job) { queuedJobs.push_back(job); };

private:
    // Fields
    ManagerBase* parent = nullptr;
    std::vector<BaseSupervisor*> children;

    std::vector<JobBase> queuedJobs;

    // Functions
    void onFrameChildren();
    void distributeJobs();
};

