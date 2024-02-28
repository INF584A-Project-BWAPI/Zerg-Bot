#include "BT_NODE.h"
#include <iostream>
#include "BT.h"

BT_NODE::~BT_NODE()
{
    delete[] Children;
}

BT_NODE::BT_NODE() {}

BT_NODE::BT_NODE(std::string name,size_t childrenMaxCount) :Name(name), Depth(0),Parent(0), ChildrenMaxCount(childrenMaxCount), ChildrenCount(0), HasBeenEvaluatedAtLeastOnce(false){
    InitChildrenArray(childrenMaxCount);
}

BT_NODE::BT_NODE(std::string name,BT_NODE* parent, size_t childrenMaxCount) :Name(name), Depth(parent!=nullptr? parent->Depth+1:0),Parent(parent), ChildrenMaxCount(childrenMaxCount), ChildrenCount(0), HasBeenEvaluatedAtLeastOnce(false) {
    InitChildrenArray(childrenMaxCount);
    if (Parent)
        Parent->AddChild(this);
}

void BT_NODE::Log(const std::string msg)
{
#ifdef BT_DEBUG_VERBOSE
    for (size_t i = 0; i < Depth; i++)
        std::cout << "\t";
    std::cout << Name << " " << GetDescription() << " " << msg << "\n";
#endif // BT_DEBUG_VERBOSE 
}

void BT_NODE::InitChildrenArray(size_t childrenMaxCount) {
    Children = new BT_NODE * [ChildrenMaxCount];
}

size_t BT_NODE::AddChild(BT_NODE* child) {
    assert(ChildrenCount < ChildrenMaxCount);
    Children[ChildrenCount++] = child;
    return ChildrenCount;
}

BT_NODE::State BT_NODE::Evaluate(void* data)
{
    HasBeenEvaluatedAtLeastOnce = true;
    return RUNNING;
}

BT_NODE::State BT_NODE::Running()
{
    Log(strState[RUNNING]);
    return RUNNING;
}

BT_NODE::State BT_NODE::Success()
{
    Log(strState[SUCCESS]);
    return SUCCESS;
}
BT_NODE::State BT_NODE::Failure()
{
    Log(strState[FAILURE]);
    return FAILURE;
}

BT_NODE::State BT_NODE::ReturnState(BT_NODE::State state)
{
    Log(strState[state]);
    return state;
}

void BT_NODE::Reset()
{
    HasBeenEvaluatedAtLeastOnce = false;

    if (!(Children != nullptr && ChildrenCount > 0)) return;

    for (size_t i = 0; i < ChildrenCount; i++)
    {
        BT_NODE* pChild = Children[i];
        if (pChild != nullptr) pChild->Reset();
    }
    Log("Reset");
}

void BT_NODE::Stop()
{
    if (!(Children != nullptr && ChildrenCount > 0)) return;

    for (size_t i = 0; i < ChildrenCount; i++)
    {
        BT_NODE *pChild = Children[i];
        if(pChild !=nullptr) pChild->Stop();
    }
    Log("Stop");
}