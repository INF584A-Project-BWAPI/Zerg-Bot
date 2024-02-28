#include "BT_COND_GREATER_THAN.h"
#include <format>


template <class T> BT_COND_GREATER_THAN<T>::BT_COND_GREATER_THAN(std::string name, BT_NODE* parent, T threshold, T& valueRef,bool strictComparison)
 :Threshold(threshold),ValueRef(valueRef), StrictComparison(strictComparison),   BT_CONDITION(name,parent,0)
{}

template <class T> BT_NODE::State BT_COND_GREATER_THAN<T>::Evaluate(void* data) {
    bool result = StrictComparison ? ValueRef > Threshold:ValueRef >= Threshold;
    if (result)  return Success();
    else return Failure();
}

template <class T> std::string BT_COND_GREATER_THAN<T>::GetDescription()
{
    return  std::format("BT_COND_GREATER_THAN {}", Threshold); 
}

template class BT_COND_GREATER_THAN<int>;
template class BT_COND_GREATER_THAN<float>;