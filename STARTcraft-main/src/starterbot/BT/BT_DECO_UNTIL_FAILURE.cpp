#include "BT_DECO_UNTIL_FAILURE.h"
#include <algorithm>

BT_DECO_UNTIL_FAILURE::BT_DECO_UNTIL_FAILURE(std::string name, BT_NODE* parent)
    : BT_DECORATOR(name,parent) 
{
}

BT_NODE::State BT_DECO_UNTIL_FAILURE::Evaluate(void* data) {

    BT_NODE::State childState = BT_DECORATOR::Evaluate(data);

	if (childState == FAILURE ) return Failure();
	else return Running();
}

std::string BT_DECO_UNTIL_FAILURE::GetDescription()
{
    return "DECO_UNTIL_FAILURE";
}