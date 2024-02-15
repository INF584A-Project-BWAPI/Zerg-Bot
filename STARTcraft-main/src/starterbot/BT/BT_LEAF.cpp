#include "BT_LEAF.h"

BT_LEAF::BT_LEAF(std::string name, BT_NODE* parent, EVALUATE_CBK evaluateCBK)
    : EvaluateCBK(evaluateCBK), BT_NODE(name,parent, 0) {}

BT_NODE::State BT_LEAF::Evaluate(void* data) {
    BT_NODE::Evaluate(data);

    assert(EvaluateCBK);
    return EvaluateCBK(data);
}
