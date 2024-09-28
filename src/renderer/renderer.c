//
// Created by wehuf on 9/26/2024.
//

#include "renderer.h"

#include "renderer_scene.h"
#include "visibility/visibility.h"

Job renderTreeExit;

static Job sortTransparentsJob;
static Job buildOpaqueDrawQueueJob;

static typedef struct {
    int index;
    int visibleIndex;
} ProcessRenderObjectInstruction;

static typedef struct {
    RenderObject* renderObject;
} VisibleOpaqueRenderObject;

static typedef struct {
    RenderObject* renderObject;
    float distanceSqr;
} VisibleTransparentRenderObject;

static VisibleOpaqueRenderObject* visibleOpaqueRenderObjects;
static VisibleTransparentRenderObject* visibleTransparentRenderObjects;

static void processOpaqueRenderObject(const ProcessRenderObjectInstruction instruction) {
    if (!isBoundingBoxInView(&renderObjects->boundingBox, &rendererFramePacket->view)) {
        visibleOpaqueRenderObjects[instruction.visibleIndex].renderObject = NULL;
        return;
    }

    visibleOpaqueRenderObjects[instruction.visibleIndex].renderObject = &renderObjects[instruction.index];
}

static void processTransparentRenderObject(const ProcessRenderObjectInstruction instruction) {
    if (!isBoundingBoxInView(&renderObjects->boundingBox, &rendererFramePacket->view)) {
        visibleTransparentRenderObjects[instruction.visibleIndex].renderObject = NULL;
        visibleTransparentRenderObjects[instruction.visibleIndex].distanceSqr = 0.0f;
        return;
    }

    visibleTransparentRenderObjects[instruction.visibleIndex].renderObject = &renderObjects[instruction.index];
    visibleTransparentRenderObjects[instruction.visibleIndex].distanceSqr = glm_vec3_distance2();

    // Calculate distance to camera
    // Set in buffer
}

// Creates jobs (deps for sort transparents and build opaque draw queue)
static void iterateVisibleObjects() {
    // Prevent workers from starting jobs before the buffers are allocated
    pthread_mutex_lock(sortTransparentsJob.mutex);
    pthread_mutex_lock(buildOpaqueDrawQueueJob.mutex);

    int numOpaqueObjects = 0;
    int numTransparentObjects = 0;

    // Create jobs and count objects
    for (int i = 0; i < arrlen(renderObjects); ++i) {
        switch (renderObjects[i].type) {
            case SIMPLE_OPAQUE:
            case UBER_OPAQUE:
                Job processOpaqueRenderObjectJob;
                initJob(&processOpaqueRenderObjectJob, NULL, processOpaqueRenderObject, { .twoNumbers = { i, numOpaqueObjects } }, "[Renderer] Process Opaque Render Object");

                arrput(buildOpaqueDrawQueueJob.deps, processOpaqueRenderObjectJob);
                numOpaqueObjects++;
                break;
            case SIMPLE_TRANSPARENT:
            case UBER_TRANSPARENT:
                Job processTransparentRenderObjectJob;
                initJob(&processTransparentRenderObjectJob, NULL, processTransparentRenderObject, { .twoNumbers = { i, numTransparentObjects } }, "[Renderer] Process Transparent Render Object");

                arrput(sortTransparentsJob.deps, processTransparentRenderObjectJob);

                numTransparentObjects++;
                break;
        }
    }

    // Allocate buffers
    visibleOpaqueRenderObjects = malloc(numOpaqueObjects * sizeof(VisibleOpaqueRenderObject));
    visibleTransparentRenderObjects = malloc(numTransparentObjects * sizeof(VisibleTransparentRenderObject));

    // Allow workers to start working on jobs and access buffers
    pthread_mutex_unlock(sortTransparentsJob.mutex);
    pthread_mutex_unlock(buildOpaqueDrawQueueJob.mutex);
}

static void sortTransparents() {

}

static void buildOpaqueDrawQueue() {

}

static void buildTransparentsDrawQueue() {

}

static void mergeDrawQueues() {

}

void initRenderer() {
    initVisibility();

    Job iterateObjectsJob;
    {
        initJob(&iterateObjectsJob, NULL, iterateVisibleObjects, { NULL }, "[RENDERER] Iterate Visible Objects");
    }

    {
        Job* sortTransparentsDeps = NULL;
        arrput(sortTransparentsDeps, iterateObjectsJob);

        initJob(&sortTransparentsJob, sortTransparentsDeps, sortTransparents, { NULL }, "[RENDERER] Sort Transparent Objects");
    }

    {
        Job* buildOpaqueDrawQueueDeps = NULL;
        arrput(buildOpaqueDrawQueueDeps, iterateObjectsJob);

        initJob(&buildOpaqueDrawQueueJob, buildOpaqueDrawQueueDeps, buildOpaqueDrawQueue, { NULL }, "[RENDERER] Build Opaque Draw Queue");
    }

    Job buildTransparentsDrawQueueJob;
    {
        Job* buildTransparentsDrawQueueDeps = NULL;
        arrput(buildTransparentsDrawQueueDeps, sortTransparentsJob);

        initJob(&buildTransparentsDrawQueueJob, buildTransparentsDrawQueueDeps, buildTransparentsDrawQueue, { NULL }, "[RENDERER] Build Transparents Draw Queue");
    }

    Job mergeDrawQueuesJob;
    {
        Job* mergeDrawQueuesDeps = NULL;
        arrput(mergeDrawQueuesDeps, buildOpaqueDrawQueueJob);
        arrput(mergeDrawQueuesDeps, buildTransparentsDrawQueueJob);

        initJob(&mergeDrawQueuesJob, mergeDrawQueuesDeps, mergeDrawQueues, { NULL }, "[RENDERER] Merge Draw Queues");
    }

    renderTreeExit = mergeDrawQueuesJob;
}

void freeRenderer() {
    freeVisibility();
    freeJobTree(&renderTreeExit);
}

void executeRenderTreeAsync() {
    for (int i = 0; i < arrlen(sortTransparentsJob.deps); ++i) {
        freeJobTree(&sortTransparentsJob.deps[i]);
    }
    arrfree(sortTransparentsJob.deps);
    sortTransparentsJob.deps = NULL;

    for (int i = 0; i < arrlen(buildOpaqueDrawQueueJob.deps); ++i) {
        freeJobTree(&buildOpaqueDrawQueueJob.deps[i]);
    }
    arrfree(buildOpaqueDrawQueueJob.deps);
    buildOpaqueDrawQueueJob.deps = NULL;

    resetJobTree(&renderTreeExit);
    executeJobTreeAsync(&renderTreeExit);
}