#include "BT_SEQUENCER.h"
#include <format>
#include <iostream>

BT_SEQUENCER:: BT_SEQUENCER(std::string name, BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(name,parent, childrenMaxCount) {};

BT_SEQUENCER::BT_SEQUENCER(std::string name, size_t childrenMaxCount) : BT_NODE(name,childrenMaxCount) {};

BT_NODE::State BT_SEQUENCER::Evaluate(void* data) {
    if (!HasBeenEvaluatedAtLeastOnce) Log("1st Evaluate");

    if (CurrentChildIndex >= ChildrenCount) return Success();

    BT_NODE::Evaluate(data);

    BT_NODE::State  childState = Children[CurrentChildIndex]->Evaluate(data);
    if (childState != SUCCESS)
        return ReturnState(childState);

    CurrentChildIndex++;
    if (CurrentChildIndex == ChildrenCount)
        return Success();
    else
        return Running();
}

void BT_SEQUENCER::Reset()
{
    BT_NODE::Reset();
    CurrentChildIndex = 0;
}

std::string BT_SEQUENCER::GetDescription()
{
    std::string str = "SEQUENCER (";
    for (size_t i = 0; i < ChildrenCount; i++)
    {
        str += " " + Children[i]->Name;
        if (i < ChildrenCount - 1)str += ",";
    }
    str += ")";
    return str;
}