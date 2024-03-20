#pragma once

#include "JobBase.h"

class JobPriorityList {
public:
    /*
    * You can think of this data struct as a queue where we can insert at the top and bottom
    * of the queue. The queue knows what the highest/lowest priority item is, thus if you add
    * a new item at the bottom it will update the priority field of JobBase.h to have this value
    * subtracting the spacing.
    */
    void clearAll();

    // Skipping items is useful if we have a job which is impossible to do right now and thus we
    // have to skip it to perform other jobs. As soon as we insert a new top priority job this
    // is reset and we start popping from the top again.
    void skipItem();
    void queueTop(JobBase& job);
    void queueBottom(JobBase& job);
    void queueItem(const JobBase job);
    void removeTop();
    void removeBottom();

    size_t size();

    bool isEmpty();
    JobBase& getTop();
    JobBase& getNextTop();

    bool canSkipItem();
    std::string getQueueInformation() const;

    int countUnitTypes(BWAPI::UnitType type);
    bool presentInTopNPositions(BWAPI::UnitType type, int maxPos);

private:
	std::deque<JobBase> queue;

	int lowest = 0;
	int highest = 0;
	int spacing = 10;
    int skipped = 0;
};

