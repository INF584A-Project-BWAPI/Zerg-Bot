#include "BT_DECO_INVERTER.h"
#include <format>

BT_DECO_INVERTER::BT_DECO_INVERTER(std::string name, BT_NODE* parent)
    :  BT_DECORATOR(name,parent) {}

BT_NODE::State BT_DECO_INVERTER::Evaluate(void* data) {

    BT_NODE::State childState = BT_DECORATOR::Evaluate(data);
	
	switch (childState)
	{
	case BT_NODE::SUCCESS:
		return Failure();
		break;
	case BT_NODE::FAILURE:
		return Success();
		break;
	default:
		return Running();
	}
}

std::string BT_DECO_INVERTER::GetDescription()
{
	return "DECO_INVERTER";
}