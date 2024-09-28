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
        initJob(&networkPollJob, NULL, networkPoll, (JobData){ NULL }, "Network Poll");
    }

    Job physicsTickJob;
    {
        Job* physicsTickJobDeps = NULL;
        arrput(physicsTickJobDeps, networkPollJob);

        initJob(&physicsTickJob, physicsTickJobDeps, executePhysicsTree, (JobData){ NULL }, "Physics Tick");
    }

    Job logicExecutionJob;
    {
        Job* logicJobDeps = NULL;
        arrput(logicJobDeps, physicsTickJob);

        initJob(&logicExecutionJob, logicJobDeps, logicExecution, (JobData){ NULL }, "Logic Execution");
    }

    Job createFramePacketJob;
    {
        Job* createFramePacketDeps = NULL;
        arrput(createFramePacketDeps, logicExecutionJob);

        initJob(&createFramePacketJob, createFramePacketDeps, generateFramePacket, (JobData){ NULL }, "Create Frame Packet");
    }

    Job* simTreeJobs = NULL;
    arrput(simTreeJobs, createFramePacketJob);

    initJobTree(&simTree, simTreeJobs, "Sim");
}

void freeSim() {
    freeJobTree(&simTree);
}

void executeSimTreeAsync() {
    resetJobTree(&simTree);
    executeJobTreeAsync(&simTree);
}