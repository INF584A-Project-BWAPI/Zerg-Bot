#pragma once

#include "JobBase.h"

class JobPriorityList {
public:
    void clearAll();
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

private:
	std::deque<JobBase> queue;

	int lowest = 0;
	int highest = 0;
	int spacing = 10;
    int skipped = 0;
};

