#include "BasesManager.h";
#include "BaseSupervisor.h";

void BasesManager::onFrame() {
    newNexus();
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
        if (children.size() == 1 && job.getAssignedManager() == ManagerType::BaseSupervisor) {
            //BaseSupervisor* child = children.at(0); // For now we assume one base
            children.at(0)->postJob(job);
        }
        if (children.size() > 1 && children.at(1)->buildnewnexus && job.getAssignedManager() == ManagerType::BaseSupervisor) {
            std::cout << "distribute between two";
            //BaseSupervisor* child1 = children.at(0); // For now we assume two base
            //BaseSupervisor* child2 = children.at(1);
            //float chance = 1/children.size();
            //for (long int i = 0; i < children.size(); i++) {

            //}
            {
                if (rand() % 10 < 6)children.at(0)->postJob(job);
                else children.at(1)->postJob(job);
            }
        }
        else {
            BaseSupervisor* child = children.at(0); // For now we assume one base
            child->postJob(job);
        }
    }


	// Remove all dsitributed jobs from the queue
	queuedJobs.clear();
}

void BasesManager::newNexus() {
    //new Base code
    bool isready = false;
    if (children.size() > 0 and children.size()<2) {
        { isready = true; }
    }
    if (isready) {
        const BaseSupervisor * child1 = children.at(0);
        //std::cout << "new nex";
        //BWAPI::TilePosition homePosition = BWAPI::Broodwar->self()->getStartLocation();
        //double minDistance = std::numeric_limits<double>::max();
        //BWAPI::TilePosition expansionPosition = homePosition;

        //for (auto& baseLocation : BWAPI::Broodwar->getStartLocations()) {
        //    if (baseLocation == homePosition) continue; // Skip the main base location
        //    //maybe not too close?
        //    double distance = homePosition.getDistance(baseLocation);
        //    if (distance < minDistance and distance>1200) {
        //        //&& isBaseLocationSuitable(baseLocation)
        //        minDistance = distance;
        //        expansionPosition = baseLocation;
        //    }
        //}
        //initialize

        BaseSupervisor* newchild = new BaseSupervisor(blackboard, true);
        if (newchild->VerifyNexus()) {
            int workertransfer = 3; int i = 0;
            for (auto& worker : BWAPI::Broodwar->self()->getUnits()) {
                if (children.at(0)->containsWorker(worker))
                {

                    children.at(0)->deleteWorker(worker);
                    newchild->addWorker(worker);
                    i += 1;
                }
                if (i > workertransfer) { break; }
            }
            std::cout << "worker plan";
            if (children.size() > 1) { BWAPI::Broodwar->printf("New Base"); }

            children.push_back(newchild);
            std::cout << "New child";
        }
        else {
            std::cout << "Regretfully not much mineral";
            delete newchild;
        }
        
    }
    
}
