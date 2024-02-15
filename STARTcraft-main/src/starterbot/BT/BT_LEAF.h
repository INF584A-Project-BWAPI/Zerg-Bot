#pragma once
#include <cassert>
#include "BT_NODE.h"

// A node with no child. Can be evaluated
//--------------------
class BT_LEAF : public BT_NODE {
public:
    typedef BT_NODE::State(*EVALUATE_CBK)(void* data);
protected:
    EVALUATE_CBK  EvaluateCBK;
public:
    BT_LEAF(std::string name, BT_NODE* parent, EVALUATE_CBK evaluateCBK);
    State Evaluate(void* data) override;
};
//----------