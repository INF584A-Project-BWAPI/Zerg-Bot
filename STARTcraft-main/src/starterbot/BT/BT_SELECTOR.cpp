#include "BT_SELECTOR.h"

BT_SELECTOR::BT_SELECTOR(std::string name, BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(name,parent, childrenMaxCount) {};
BT_SELECTOR::BT_SELECTOR(std::string name, size_t childrenMaxCount) : BT_NODE(name,childrenMaxCount) {};

BT_NODE::State BT_SELECTOR::Evaluate(void* data) {
    if (!HasBeenEvaluatedAtLeastOnce) Log("1st Evaluate");

    if (CurrentChildIndex >= ChildrenCount) return Failure();

    BT_NODE::Evaluate(data);

    BT_NODE::State  childState = Children[CurrentChildIndex]->Evaluate(data);
    if (childState != FAILURE)
        return ReturnState(childState);

    CurrentChildIndex++;
    if (CurrentChildIndex == ChildrenCount)
        return Failure();
    else
        return Running();
}

void BT_SELECTOR::Reset()
{
    BT_NODE::Reset();
    CurrentChildIndex = 0;
}

std::string BT_SELECTOR::GetDescription()
{
    std::string str = "SELECTOR (";
    for (size_t i = 0; i < ChildrenCount; i++)
    {
        str += " " + Children[i]->Name;
        if (i < ChildrenCount - 1)str += ",";
    }
    str += ")";
    return str;
}