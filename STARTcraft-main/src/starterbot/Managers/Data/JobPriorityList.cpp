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

    if (queue.empty()) {
        highest = p;
        lowest = p;
    }

    if (p <= lowest) {
        queue.push_front(job);
    }
    else {
        queue.push_back(job);
    }

    if ((queue.size() > 1) && (p < highest) && (p > lowest)) {
        std::sort(queue.begin(), queue.end());
    }

    highest = (p > highest) ? p : highest;
    lowest = (p < lowest) ? p : lowest;
}

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

JobBase& JobPriorityList::getTop() {
    skipped = 0;

    return queue.back();
}

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
