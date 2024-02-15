#include "BT_ACTION_WAIT.h"
#include <iostream>
#include <format>

BT_ACTION_WAIT::BT_ACTION_WAIT(std::string name, BT_NODE* parent,float duration)
    :Duration(duration), BT_ACTION(name,parent) {}

BT_NODE::State BT_ACTION_WAIT::Evaluate(void* data)
{
    if (!HasBeenEvaluatedAtLeastOnce)
    {
        StartTime = std::chrono::system_clock::now();
        Log("1st Evaluate");
    }

    BT_NODE::Evaluate(data);

    auto now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedSeconds = now - StartTime;

    if (elapsedSeconds.count() >= Duration)
        return Success();
    else
        Running();
}

std::string BT_ACTION_WAIT::GetDescription()
{
    return std::format("WAIT ACTION {}s", Duration);
}
