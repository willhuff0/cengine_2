//
// Created by wehuf on 9/26/2024.
//

#include "simulation.h"

#include "sim_tree.h"
#include "../engine/input.h"

void initSim() {
    initSimTree();
}
void freeSim() {
    freeSimTree();
}

void simTick() {
    executeSimTree();
}