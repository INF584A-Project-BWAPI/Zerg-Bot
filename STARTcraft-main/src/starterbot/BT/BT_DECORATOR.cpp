#include "BT_DECORATOR.h"

BT_DECORATOR::BT_DECORATOR(std::string name, BT_NODE* parent)
    : BT_NODE(name,parent, 1) {}

BT_NODE::State BT_DECORATOR::Evaluate(void* data) {
    BT_NODE::Evaluate(data);
    assert(Children && ChildrenCount==1);
    return Children[0]->Evaluate(data);
}

std::string BT_DECORATOR::GetDescription()
{
    return "DECORATOR";
}