#include "BT_DECO_RETURN_SUCCESS.h"
#include <algorithm>

BT_DECO_RETURN_SUCCESS::BT_DECO_RETURN_SUCCESS(std::string name, BT_NODE* parent)
    : BT_DECORATOR(name,parent) 
{
}

BT_NODE::State BT_DECO_RETURN_SUCCESS::Evaluate(void* data) {

    BT_NODE::State childState = BT_DECORATOR::Evaluate(data);

	if (childState == RUNNING ) return Running();
	else return Success();
}

std::string BT_DECO_RETURN_SUCCESS::GetDescription()
{
    return "DECO_RETURN_SUCCESS";
}