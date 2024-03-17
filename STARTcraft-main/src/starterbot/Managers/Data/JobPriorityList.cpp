#include "JobPriorityList.h"
#include <cassert>


void JobPriorityList::clearAll() {
	queue.clear();

	lowest = 0;
	highest = 0;
}

void JobPriorityList::skipItem() {
    assert(canSkipItem());

    skipped++;
}

void JobPriorityList::queueTop(JobBase& job) {
    job.setPriority(highest + spacing);

    queueItem(job);
}

void JobPriorityList::queueBottom(JobBase& job) {
    job.setPriority(lowest - 10);

    queueItem(job);
}

void JobPriorityList::queueItem(const JobBase job) {
    int p = job.getPriority();

    // If queue is empty then lowest priority is the same as highest
    if (queue.empty()) {
        highest = p;
        lowest = p;
    }

    // If priority is lower than lowest insert at the front, else at the back
    if (p <= lowest) {
        queue.push_front(job);
    }
    else {
        queue.push_back(job);
    }

    // Sort the queue deque based in job priority
    if ((queue.size() > 1) && (p < highest) && (p > lowest)) {
        std::sort(queue.begin(), queue.end());
    }

    // Update lowest and highest priority to reflect this addition
    highest = (p > highest) ? p : highest;
    lowest = (p < lowest) ? p : lowest;
}

// Removes top item, or if we skipped some items we remove the item we skipped to
void JobPriorityList::removeTop() {
    queue.erase(queue.begin() + queue.size() - 1 - skipped);

    highest = queue.empty() ? 0 : queue.back().getPriority();
    lowest = queue.empty() ? 0 : lowest;
}

void JobPriorityList::removeBottom() {
    queue.pop_back();

    highest = queue.empty() ? 0 : queue.back().getPriority();
    lowest = queue.empty() ? 0 : lowest;
}

size_t JobPriorityList::size()
{
	return queue.size();
}

bool JobPriorityList::isEmpty() {
	return queue.empty();
}

// Get the top element regardless of skipped items, and reset the skipped items
JobBase& JobPriorityList::getTop() {
    skipped = 0;

    return queue.back();
}

// Is used to get the top job which we have skipped to 
JobBase& JobPriorityList::getNextTop()
{
    assert(queue.size() - 1 - skipped >= 0);

    return queue[queue.size() - 1 - skipped];
}

bool JobPriorityList::canSkipItem() {
    bool bigEnough = queue.size() > (size_t)(1 + skipped);

    if (!bigEnough) {
        return false;
    }

    bool highestNotBlocking = !queue[queue.size() - 1 - skipped].blocking;

    return highestNotBlocking;
}

// Return a string for the jobs present in the job such that we can debug its contents.
std::string JobPriorityList::getQueueInformation() const
{
    size_t reps = queue.size() < 30 ? queue.size() : 30;
    std::stringstream ss;

    for (size_t i(0); i < reps; i++) {
        const BWAPI::UnitType& type = queue[queue.size() - 1 - i].getUnit();
        ss << type.getName().c_str() << "\n";
    }

    return ss.str();
}

int JobPriorityList::countUnitTypes(BWAPI::UnitType type) {
    int count = 0;

    for (JobBase& job : queue) {
        if (job.getUnit() == type)
            count++;
    }

    return count;
}
