#include "ArmyManager.h"
#include <iostream>
#include "Data.h"
#include "AttackGroundUnitBT.h"

void ArmyManager::onFrame() {

    squads = blackboard.squads;

    if (!queuedJobs.isEmpty()) {
        // Build queued buildings
        // looks at highest priority item without popping from queue yet
        JobBase job = queuedJobs.getTop();

        // Do something
        //std::cout << "We have a Army job: " << job.getUnit() << std::endl;
    }

    //for (BWAPI::Unitset squad: squads){
        //BWAPI::Broodwar->sendText(squad)
    //}
    //if (squads.empty()) {
        //BWAPI::Broodwar->sendText("SQUADS EMPTY");
    //}

    for (BWAPI::Unitset enemies: blackboard.scout_info) {
        for (auto& enemy : enemies) {
            if (!blackboard.enemyBuilding.contains(enemy) && enemy->getType().isBuilding() && enemy->getType().isResourceDepot()) {
                blackboard.enemyBuilding.insert(enemy);
            }
        }
    }

    if (!squads.empty()){
        /*
        BWAPI::Unit position;
        for (BWAPI::Unit enemy : blackboard.enemyBuilding) {
            if (enemy) {
                position = enemy;
                break;
            }
        }
        for (auto& unit : squads[0]) {
            unit->attack(position);
        
        */
        attackGroundUnitBT = new AttackGroundUnitBT();
        if (attackGroundUnitBT->Evaluate(&blackboard) == BT_NODE::SUCCESS) {
            //std::cout << "Executed BT...\n";
            delete (AttackGroundUnitBT*)attackGroundUnitBT;
            attackGroundUnitBT = nullptr;
        }
        
        /*
        //std::vector<BWAPI::Unit> unitVector;
        for (BWAPI::Unit enemy : blackboard.enemyBuilding) {
            BWAPI::Position position = enemy->getPosition();
            break;
        }
        //BWAPI::Unit enemy = *blackboard.enemyBuilding.begin();
        //BWAPI::Position position = enemy->getPosition();
        for (auto& unit : squads[0]) {
                unit->attack(position);
        }*/
    }




    /*
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot && unit->isIdle() && !zealots.contains(unit)) {
            std::cout << zealots.contains(unit) << std::endl;
            zealots.insert(unit);
            BWAPI::Broodwar->sendText("Unit (%s) id (%d) added to %s",
                unit->getType().getName().c_str(),
                unit->getID(),
                "zealots");
        }
    }


    if (zealots.size() == 5) {
        BWAPI::Broodwar->sendText("Current Zealots in Unitset:");
        for (auto& zealot : zealots) {
            BWAPI::Broodwar->sendText("Unit (%s) id (%d)",
                zealot->getType().getName().c_str(),
                zealot->getID());
        }
        for (auto& unit : zealots) {
            unit->move(BWAPI::Position(1000, 1000));
        }
    }
    
    if (squads.empty()) {
        squads.push_back(zealots);
    }
    */


    /*
    if (squads.empty()) { // Controlla se non ci sono squad gi?create
        createPatrolSquad();
    }

    // Controlla se ci sono unit?inattive e le assegna alla squad di pattuglia esistente.
    Squad* patrolSquad = nullptr;
    for (Squad& squad : squads) {
        if (squad.getStatus() == SquadStatus::Patrol) {
            patrolSquad = &squad;
            break;
        }
    }

    // Se non esiste una squad di pattuglia, ne crea una nuova.
    //if (!patrolSquad) {
    //   createPatrolSquad();
    //    patrolSquad = &squads.back(); // La nuova squad di pattuglia ?l'ultima creata
    //}

    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->isIdle() && !unit->getType().isBuilding() && !unit->getType().isWorker() && unit->getType() == BWAPI::UnitTypes::Protoss_Zealot) {
            patrolSquad->addUnit(unit);
        }
    }*/
}

/*void ArmyManager::createPatrolSquad() {
    Squad newSquad(nextSquadId++); // Crea una nuova squad con un ID univoco
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        // Verifica che l'unit?sia inattiva, non sia un edificio, e non sia un lavoratore
        if (unit->isIdle() && !unit->getType().isBuilding() && !unit->getType().isWorker()) {
            newSquad.addUnit(unit);
        }
    }
    if (!newSquad.getUnits().empty()) { // Aggiungi la squad solo se contiene unit?
        newSquad.updateStatus(SquadStatus::Patrol); // Imposta la squad in modalit?pattuglia
        squads.push_back(newSquad); // Aggiungi la squad all'elenco delle squad gestite
    }
}

void ArmyManager::sendPatrolSquadToPosition(const BWAPI::Position& targetPosition) {
    for (Squad& squad : squads) {
        if (squad.getStatus() == SquadStatus::Patrol) {
            squad.moveTo(targetPosition);
            break; // Interrompe il ciclo dopo aver inviato la prima squadra di pattuglia trovata
        }
    }
}*/
