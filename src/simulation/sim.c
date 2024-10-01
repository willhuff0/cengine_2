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

JobTree simTree;

void initSim() {
    Job networkPollJob;
    {
        initJob(&networkPollJob, 0, NULL, networkPoll, (JobData){ NULL }, "Network Poll");
    }

    Job physicsTickJob;
    {
        Job* physicsTickJobDeps = malloc(1 * sizeof(Job));
        physicsTickJobDeps[0] = networkPollJob;

        initJob(&physicsTickJob, 1, physicsTickJobDeps, executePhysicsTree, (JobData){ NULL }, "Physics Tick");
    }

    Job logicExecutionJob;
    {
        Job* logicJobDeps = malloc(1 * sizeof(Job));
        logicJobDeps[0] = physicsTickJob;

        initJob(&logicExecutionJob, 1, logicJobDeps, logicExecution, (JobData){ NULL }, "Logic Execution");
    }

    Job createFramePacketJob;
    {
        Job* createFramePacketDeps = malloc(1 * sizeof(Job));;
        createFramePacketDeps[0] = logicExecutionJob;

        initJob(&createFramePacketJob, 1, createFramePacketDeps, generateFramePacket, (JobData){ NULL }, "Create Frame Packet");
    }

    Job* simTreeJobs = malloc(1 * sizeof(Job));
    simTreeJobs[0] = createFramePacketJob;

    initJobTree(&simTree, 1, simTreeJobs, "Sim");
}

void freeSim() {
    freeJobTree(&simTree);
}

void executeSimTreeAsync() {
    resetJobTree(&simTree);
    executeJobTreeAsync(&simTree);
}