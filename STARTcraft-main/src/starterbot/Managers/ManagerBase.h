#pragma once

#include "Data/JobBase.h"


class ManagerBase {
public:
    ManagerBase(ManagerType managerType) noexcept : managerType(managerType) {};

    // Getters
    ManagerType getManagerType() const noexcept { return managerType; };

private:
    // Fields
    ManagerType managerType;
};