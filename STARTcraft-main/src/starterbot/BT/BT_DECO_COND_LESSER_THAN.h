#pragma once
#include <cassert>
#include "BT_DECO_CONDITION.h"


// Specific conditional decorator task implementing the following condition: "a value is lesser than a constant threshold" 
// The Value reference and the Threshold constant are passed to the constructor

// Reminder of the definition of a conditional decorator 
// If the  conditional decorator task returns success then the child task is run and the child status is returned. If the conditional decorator task does not
// return success then the child task is not run and a failure status is immediately returned.
// 
//--------------------
template <class T> class BT_DECO_COND_LESSER_THAN : public BT_DECO_CONDITION {

protected:
    T Threshold;
    T& ValueRef;
    bool StrictComparison;
public:
    BT_DECO_COND_LESSER_THAN(std::string name, BT_NODE* parent,T threshold,T& ValueRef,bool strictComparison);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------

