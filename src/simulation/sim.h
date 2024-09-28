//
// Created by wehuf on 9/26/2024.
//

#ifndef SIM_H
#define SIM_H

#include "../jobs/jobs.h"

extern JobTree simTree;

void initSim();
void freeSim();

void executeSimTreeAsync();

#endif //SIM_H
