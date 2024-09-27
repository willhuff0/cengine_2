//
// Created by wehuf on 9/26/2024.
//

#include "sim.h"

#include "../engine/input.h"
#include "../jobs/jobs.h"
#include "../renderer/renderer.h"
#include "logic/logic.h"
#include "network/network.h"
#include "physics/physics.h"

static Job simTreeExit;

void initSim() {
    Job inputJob;
    {
        initJob(&inputJob, NULL, inputSimStartTick, "[JOB: (Sim) Input]");
    }

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
        arrput(logicJobDeps, inputJob);
        arrput(logicJobDeps, physicsTickJob);

        initJob(&logicExecutionJob, logicJobDeps, logicExecution, "[JOB: (Sim) Logic Execution");
    }

    Job cloneRenderDataJob;
    {
        Job* cloneRenderDataDeps = NULL;
        arrput(cloneRenderDataDeps, logicExecutionJob);

        initJob(&cloneRenderDataJob, cloneRenderDataDeps, NULL, "[JOB: (Sim) Clone Render Data");
    }

    Job renderFrameJob;
    {
        Job* renderFrameJobDeps = NULL;
        arrput(renderFrameJobDeps, cloneRenderDataJob);

        initJob(&renderFrameJob, renderFrameJobDeps, executeRenderTree, "[JOB: (Sim) Render Frame");
    }

    simTreeExit = renderFrameJob;
}

void freeSim() {
    freeJobTree(&simTreeExit);
}

void executeSimTree() {
    resetJobTree(&simTreeExit);
    executeJobTree(&simTreeExit);
}