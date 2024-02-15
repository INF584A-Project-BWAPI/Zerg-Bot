#include "BT_DECO_COND_LESSER_THAN.h"
#include <format>

template <class T> BT_DECO_COND_LESSER_THAN<T>::BT_DECO_COND_LESSER_THAN(std::string name, BT_NODE* parent, T threshold, T& valueRef,bool strictComparison)
 :Threshold(threshold),ValueRef(valueRef), StrictComparison(strictComparison), BT_DECO_CONDITION(name,parent,0)
{}

template <class T> BT_NODE::State BT_DECO_COND_LESSER_THAN<T>::Evaluate(void* data) {
    bool result = StrictComparison ? ValueRef < Threshold:ValueRef <= Threshold;
    if (result)  return ReturnState(BT_DECORATOR::Evaluate(data));
    else return Failure();
}

template <class T> std::string BT_DECO_COND_LESSER_THAN<T>::GetDescription()
{
    return  std::format("BT_DECO_COND_LESSER_THAN {}", Threshold); 
}

// Add your specialized template instantiations here
template class BT_DECO_COND_LESSER_THAN<int>;
template class BT_DECO_COND_LESSER_THAN<float>;