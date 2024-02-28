#include "BT_PARALLEL_SEQUENCER.h"

BT_PARALLEL_SEQUENCER::~BT_PARALLEL_SEQUENCER()
{
    delete[] ChildrenStatus;
}

BT_PARALLEL_SEQUENCER::BT_PARALLEL_SEQUENCER(std::string name, BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(name,parent, childrenMaxCount)
{
    ChildrenStatus = new BT_NODE::State[childrenMaxCount];
    for (size_t i = 0; i < childrenMaxCount; i++) ChildrenStatus[i] = RUNNING;
};

BT_PARALLEL_SEQUENCER::BT_PARALLEL_SEQUENCER(std::string name, size_t childrenMaxCount) : BT_NODE(name,childrenMaxCount)
{
    ChildrenStatus = new BT_NODE::State[childrenMaxCount];
    for (size_t i = 0; i < childrenMaxCount; i++) ChildrenStatus[i] = RUNNING;
};

BT_NODE::State BT_PARALLEL_SEQUENCER::Evaluate(void* data) {
    if (!HasBeenEvaluatedAtLeastOnce) Log("1st Evaluate");

    BT_NODE::Evaluate(data);

    for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) {
        if (ChildrenStatus[childIdx] == BT_NODE::State::RUNNING)
        {
            BT_NODE::State  childState = Children[childIdx]->Evaluate(data);
            ChildrenStatus[childIdx] = childState;

            // if one child returns failure, then return failure
            if (childState == BT_NODE::State::FAILURE) 
            {
                for (size_t i = 0; i < ChildrenCount; i++)
                    if (ChildrenStatus[i] == BT_NODE::State::RUNNING)
                        Children[i]->Stop();
                return Failure();
            }
        }
    }

    // no failure 
    // but if one child returns running, then returns running
    for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) 
        if (ChildrenStatus[childIdx] == RUNNING) 
            return Running();

    // return success when all children have finished in success
    return Success();
}

void BT_PARALLEL_SEQUENCER::Reset()
{
    BT_NODE::Reset();
    assert(ChildrenStatus);
    for (size_t i = 0; i < ChildrenCount; i++)
        ChildrenStatus[i] = RUNNING;
}

std::string BT_PARALLEL_SEQUENCER::GetDescription()
{
    std::string str = "PARALLEL_SEQUENCER (";
    for (size_t i = 0; i < ChildrenCount; i++)
    {
        str += " " + Children[i]->Name;
        if (i < ChildrenCount - 1)str += ",";
    }
    str += ")";
    return str;
}