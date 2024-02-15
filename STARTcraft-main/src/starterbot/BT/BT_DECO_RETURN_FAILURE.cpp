#include "BT_DECO_RETURN_FAILURE.h"
#include <algorithm>

BT_DECO_RETURN_FAILURE::BT_DECO_RETURN_FAILURE(std::string name, BT_NODE* parent)
    : BT_DECORATOR(name,parent) 
{
}

BT_NODE::State BT_DECO_RETURN_FAILURE::Evaluate(void* data) {

    BT_NODE::State childState = BT_DECORATOR::Evaluate(data);

	if (childState == RUNNING ) return Running();
	else return Failure();
}

std::string BT_DECO_RETURN_FAILURE::GetDescription()
{
    return "DECO_RETURN_FAILURE";
}