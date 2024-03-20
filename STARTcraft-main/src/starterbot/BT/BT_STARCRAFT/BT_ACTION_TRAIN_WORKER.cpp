#include "BT_ACTION_TRAIN_WORKER.h"
#include "Tools.h"
#include "Data.h"

BT_ACTION_TRAIN_WORKER::BT_ACTION_TRAIN_WORKER(std::string name,BT_NODE* parent)
    :  BT_ACTION(name,parent) {}

BT_NODE::State BT_ACTION_TRAIN_WORKER::Evaluate(void* data)
{
    return ReturnState(TrainWorker(data));
}

std::string BT_ACTION_TRAIN_WORKER::GetDescription()
{
    return "TRAIN WORKER";
}


BT_NODE::State BT_ACTION_TRAIN_WORKER::TrainWorker(void* data)
{
    Data* pData = (Data*)data;

    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const BWAPI::Unit myDepot = Tools::GetDepot();

    // if we have a valid depot unit and it's currently not training something, train a worker
    // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
    if (myDepot && !myDepot->isTraining()) { 
        myDepot->train(workerType); 
        BWAPI::Error error = BWAPI::Broodwar->getLastError();
        if(error!=BWAPI::Errors::None)
            return BT_NODE::FAILURE;
        else return BT_NODE::SUCCESS;
    }

    return BT_NODE::FAILURE;
}