#include "BasesManager.h";
#include "BaseSupervisor.h";
#include "AuxBaseSupervisor.h";
void BasesManager::onFrame() {
    //newNexus();
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
//void BasesManager::newNexus() {
//    // Only expand to a second base for now
//    if (children.size() > 0 && children.size() < 2) {
//        const BaseSupervisor* baseMain = children.at(0);
//        // New Forward Operating Base (FOB because it has a different role in-game)
//        BaseSupervisor* baseFOB = new BaseSupervisor(blackboard, true);
//
//        baseFOB->verifyNexus();
//        {
//            int workersToTransfer = 1;
//            int i = 0;
//            std::cout << "new base ";
//            for (const BWAPI::Unit& worker : BWAPI::Broodwar->self()->getUnits()) {
//                if (i > workersToTransfer)
//                    break;
//
//                if (!children.at(0)->containsWorker(worker)) {
//                    //children.at(0)->deleteWorker(worker); // Remove worker from main base
//                    std::cout << "to assign";
//                    baseFOB->addWorker(worker); // Asign worker to new FOB
//                    i += 1;
//                }
//            }
//
//            children.push_back(baseFOB);
//        }
//        //else {
//        //    delete baseFOB;
//        //}
//    }
//    
//}