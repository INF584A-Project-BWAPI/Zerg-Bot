#pragma once

#include "Data/JobBase.h"
#include "../Blackboard.h"

class ManagerBase {
public:
    ManagerBase(ManagerType managerType, Blackboard& blackboard) noexcept
        : managerType(managerType)
        , blackboard(blackboard) {};

    ManagerType getManagerType() const noexcept { return managerType; };

    // Fields
    Blackboard& blackboard;

private:
    // Fields
    ManagerType managerType;
};