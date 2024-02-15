#include "BT_DECO_REPEATER.h"
#include <algorithm>
#include <format>

BT_DECO_REPEATER::BT_DECO_REPEATER(std::string name, BT_NODE* parent, size_t nMaxRepeats, bool repeatForever, bool endOnFailure,bool resetChildOnRepeat)
    : NMaxRepeats(std::min(nMaxRepeats,(size_t)1)),RepeatForever(repeatForever),EndOnFailure(endOnFailure),ResetChildOnRepeat(resetChildOnRepeat),  BT_DECORATOR(name,parent) 
{
	NRepeats =0;
}

BT_NODE::State BT_DECO_REPEATER::Evaluate(void* data) {
	if (!HasBeenEvaluatedAtLeastOnce) Log("1st Evaluate");

    BT_NODE::State childState = BT_DECORATOR::Evaluate(data);

	//
	if(childState != RUNNING 
		&& ResetChildOnRepeat
		&& (RepeatForever || NRepeats < NMaxRepeats))
		Children[0]->Reset();

	//
	if (childState == FAILURE && EndOnFailure) return Failure();

	if (childState == RUNNING || RepeatForever) return Running();

	NRepeats++;

	if (NRepeats >= NMaxRepeats)
		return ReturnState(childState);
	else return Running();
}

void BT_DECO_REPEATER::Reset()
{
	BT_NODE::Reset();
	NRepeats = 0;
}

std::string BT_DECO_REPEATER::GetDescription()
{
	return std::format("DECO_REPEATER (nMaxRepeats = {}, repeatForever = {}, endOnFailure = {}",NMaxRepeats,RepeatForever,EndOnFailure);
}