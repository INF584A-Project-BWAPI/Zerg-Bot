#include "JobBase.h"

JobBase::JobBase(int priority, ManagerType assignedManager) 
	: priority(priority), assignedManager(assignedManager) {};

// Setters
void JobBase::setPriority(int priority) { priority = priority; };

// Getters
int JobBase::getPriority() const { return priority; };
ManagerType JobBase::getAssignedManager() const { return assignedManager; }