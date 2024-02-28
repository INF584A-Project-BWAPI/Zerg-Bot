#pragma once
#include <cassert>
#include <string>

// A base node class
//--------------------
class BT_NODE {
public:
    enum State {
        RUNNING,
        SUCCESS,
        FAILURE
    };
    const char* strState[3] = { "RUNNING", "SUCCESS", "FAILURE"};
    //---
    std::string Name;

protected:
    int Depth=0;

    BT_NODE** Children;
    size_t    ChildrenCount;
    size_t    ChildrenMaxCount;
    BT_NODE* Parent;
    bool HasBeenEvaluatedAtLeastOnce;
    //---
    virtual ~BT_NODE();
    //---
    BT_NODE(std::string name,size_t childrenMaxCount); 
    //---
    BT_NODE(std::string name,BT_NODE* parent, size_t childrenMaxCount);
    //---
    State Running();
    State Success();
    State Failure();
    State ReturnState(State state);
private:
    BT_NODE();
    //---
    void InitChildrenArray(size_t childrenMaxCount);
    //---
public:
    virtual State Evaluate(void* data);
    virtual void Reset();
    virtual void Stop();

    virtual std::string GetDescription() = 0;
    void Log(std::string msg);
    //---
    size_t AddChild(BT_NODE* child);
    //---
};
//----------