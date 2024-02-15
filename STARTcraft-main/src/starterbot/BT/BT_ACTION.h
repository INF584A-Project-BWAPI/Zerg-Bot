#pragma once
#include <cassert>
#include "BT_LEAF.h"

// action node, same as leaf
//--------------------
class BT_ACTION : public BT_NODE {
public:
    BT_ACTION(std::string name, BT_NODE* parent);
};
//----------