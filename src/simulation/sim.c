//
// Created by wehuf on 9/26/2024.
//

#include "sim.h"

#include "frame_packet_generator.h"
#include "../engine/input.h"
#include "../renderer/renderer.h"
#include "logic/logic.h"
#include "network/network.h"
#include "physics/physics.h"

Job simTreeExit;

void initSim() {
    Job networkPollJob;
    {
        initJob(&networkPollJob, NULL, networkPoll, "[JOB: (Sim) Network Poll");
    }

    Job physicsTickJob;
    {
        Job* physicsTickJobDeps = NULL;
        arrput(physicsTickJobDeps, networkPollJob);

        initJob(&physicsTickJob, physicsTickJobDeps, executePhysicsTree, "[JOB: (Sim) Physics Tick");
    }

    Job logicExecutionJob;
    {
        Job* logicJobDeps = NULL;
        arrput(logicJobDeps, physicsTickJob);

        initJob(&logicExecutionJob, logicJobDeps, logicExecution, "[JOB: (Sim) Logic Execution");
    }

    Job createFramePacketJob;
    {
        Job* createFramePacketDeps = NULL;
        arrput(createFramePacketDeps, logicExecutionJob);

        initJob(&createFramePacketJob, createFramePacketDeps, generateFramePacket, "[JOB: (Sim) Create Frame Packet");
    }

    simTreeExit = createFramePacketJob;
}

void freeSim() {
    freeJobTree(&simTreeExit);
}

void executeSimTreeAsync() {
    resetJobTree(&simTreeExit);
    executeJobTreeAsync(&simTreeExit);
}