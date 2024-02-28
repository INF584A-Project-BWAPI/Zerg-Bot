#include "BT_ACTION_LOG.h"
#include <iostream>
#include <format>

BT_ACTION_LOG::BT_ACTION_LOG(std::string name, BT_NODE* parent, std::string msg)
    :Msg(msg), BT_ACTION(name,parent) {}

BT_NODE::State BT_ACTION_LOG::Evaluate(void* data)
{
    std::cout<<Msg << "\n"; 
    return Success();
}

std::string BT_ACTION_LOG::GetDescription()
{
    return std::format("LOG ACTION {}", Msg);
}
