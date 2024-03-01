#include "BasesManager.h";
#include "BaseSupervisor.h";

void BasesManager::onFrame() {
	onFrameChildren();
}

void BasesManager::onFrameChildren() {
	distributeJobs();

	for (BaseSupervisor* child : children) {
		child->onFrame();
	}
}

void BasesManager::distributeJobs() {
	for (JobBase job : queuedJobs) {
		if (children.size() > 0 && job.getAssignedManager() == ManagerType::BaseSupervisor) {
			BaseSupervisor* child = children.at(0); // For now we assume one base
			child->postJob(job);
		}
	}

	// Remove all dsitributed jobs from the queue
	queuedJobs.clear();
}
