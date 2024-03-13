#pragma once

#include "ManagerBase.h"

/*
What kind of jobs can we have in a base?
    - Building
    - Resource gathering
*/


class BuildManager : public ManagerBase
{
public:
    BuildManager() : ManagerBase() {};

    void onFrame();
};

