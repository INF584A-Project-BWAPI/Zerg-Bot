#include "BT_DECO_UNTIL_SUCCESS.h"
#include <algorithm>

BT_DECO_UNTIL_SUCCESS::BT_DECO_UNTIL_SUCCESS(std::string name, BT_NODE* parent)
    : BT_DECORATOR(name,parent) 
{
}

BT_NODE::State BT_DECO_UNTIL_SUCCESS::Evaluate(void* data) {

    BT_NODE::State childState = BT_DECORATOR::Evaluate(data);

	if (childState == SUCCESS ) return Success();
	else return Running();
}

std::string BT_DECO_UNTIL_SUCCESS::GetDescription()
{
    return "DECO_UNTIL_SUCCESS";
}