//
// Created by wehuf on 9/26/2024.
//

#include "renderer.h"

#include "frame_packet.h"
#include "renderer_object.h"
#include "../engine/draw_system.h"

JobTree renderTree;

static Job* buildOpaqueDrawQueueJob;
static Job* buildTransparentsDrawQueueJob;

typedef struct {
    RendererObjectID rendererObject;
    int processedRenderObjectIndex;
} ProcessRendererObjectInstruction;

typedef struct {
    RendererObjectID rendererObject;
} ProcessedOpaqueRendererObject;

typedef struct {
    RendererObjectID rendererObject;
    float distanceSqr;
} ProcessedTransparentRendererObject;

static int numOpaqueRendererObjects;
static ProcessedOpaqueRendererObject* processedOpaqueRendererObjects;

static int numTransparentRendererObjects;
static ProcessedTransparentRendererObject* processedTransparentRendererObjects;

static int uberMeshOffset;
static int uberMaterialOffset;

static void processOpaqueRenderObject(const ProcessRendererObjectInstruction instruction) {
#ifdef RENDERER_ENABLE_CULLING
    if (!aabbIsInFrustum(GET_RENDERER_OBJECT_AABB(lookupRendererObject(instruction.rendererObject)), lookupRendererObject(instruction.rendererObject)->transform.matrix, rendererFramePacket->view.frustum)) {
        processedOpaqueRendererObjects[instruction.processedRenderObjectIndex].rendererObject = -1;
        return;
    }
#endif

    processedOpaqueRendererObjects[instruction.processedRenderObjectIndex].rendererObject = instruction.rendererObject;
}

static void processTransparentRenderObject(const ProcessRendererObjectInstruction instruction) {
#ifdef RENDERER_ENABLE_CULLING
    if (!aabbIsInFrustum(GET_RENDERER_OBJECT_AABB(lookupRendererObject(instruction.rendererObject)), lookupRendererObject(instruction.rendererObject)->transform.matrix, rendererFramePacket->view.frustum)) {
        processedTransparentRendererObjects[instruction.processedRenderObjectIndex].rendererObject = -1;
        processedTransparentRendererObjects[instruction.processedRenderObjectIndex].distanceSqr = -1.0f;
        return;
    }
#endif

    const float distanceSqr = glm_vec3_distance2(rendererFramePacket->view.pos, lookupRendererObject(instruction.rendererObject)->transform.matrix);

    processedTransparentRendererObjects[instruction.processedRenderObjectIndex].rendererObject = instruction.rendererObject;
    processedTransparentRendererObjects[instruction.processedRenderObjectIndex].distanceSqr = distanceSqr;
}

// Creates jobs (deps for sort transparents and build opaque draw queue)
static void iterateVisibleObjects() {
    // Prevent workers from starting jobs before the buffers are allocated
    lockJobTree(&renderTree);

    numOpaqueRendererObjects = 0;
    numTransparentRendererObjects = 0;

    // Create jobs and count objects
    for (int i = 0; i < arrlen(rendererObjects); ++i) {
        switch (rendererObjects[i].type) {
            case SIMPLE_OPAQUE_RENDERER_OBJECT:
            case UBER_OPAQUE_RENDERER_OBJECT:
                Job processOpaqueRenderObjectJob;
                initJob(&processOpaqueRenderObjectJob, 0, NULL, processOpaqueRenderObject, (JobData){ .twoNumbers = { i, numOpaqueRendererObjects } }, "[Renderer] Process Opaque Renderer Object");

                arrput(buildOpaqueDrawQueueJob->deps.dynamicDeps, processOpaqueRenderObjectJob);

                numOpaqueRendererObjects++;
                break;
            case SIMPLE_TRANSPARENT_RENDERER_OBJECT:
            case UBER_TRANSPARENT_RENDERER_OBJECT:
                Job processTransparentRenderObjectJob;
                initJob(&processTransparentRenderObjectJob, 0, NULL, processTransparentRenderObject, (JobData){ .twoNumbers = { i, numTransparentRendererObjects } }, "[Renderer] Process Transparent Renderer Object");

                arrput(buildTransparentsDrawQueueJob->deps.dynamicDeps, processTransparentRenderObjectJob);

                numTransparentRendererObjects++;
                break;
        }
    }

    // Allocate buffers
    if (processedOpaqueRendererObjects != NULL) free(processedOpaqueRendererObjects);
    if (processedTransparentRendererObjects != NULL) free(processedTransparentRendererObjects);

    processedOpaqueRendererObjects = malloc(numOpaqueRendererObjects * sizeof(ProcessedOpaqueRendererObject));
    processedTransparentRendererObjects = malloc(numTransparentRendererObjects * sizeof(ProcessedTransparentRendererObject));

    uberMeshOffset = arrlen(assets.simpleMeshes);
    uberMaterialOffset = arrlen(assets.simpleMaterials);

    // Allow workers to start working on jobs and access buffers
    unlockJobTree(&renderTree);
}

static int compareOpaqueRendererObjectsByMaterialThenShaderThenMesh(const ProcessedOpaqueRendererObject* a, const ProcessedOpaqueRendererObject* b) {
    // Sort culled objects last
    if (a->rendererObject == -1 || b->rendererObject == -1) {
        return b->rendererObject - a->rendererObject;
    }

    int aMeshID;
    int aMaterialID;
    int aShaderID;
    if (IS_RENDERER_OBJECT_SIMPLE(lookupRendererObject(a->rendererObject))) {
        aMeshID = lookupRendererObject(a->rendererObject)->mesh.simpleMesh;
        aMaterialID = lookupSimpleMesh(aMeshID)->material;
        aShaderID = lookupSimpleMaterial(aMaterialID)->shader;
    } else {
        aMeshID = lookupRendererObject(a->rendererObject)->mesh.uberMesh;
        //aMaterialID = uberMaterialOffset + lookupUberMesh(aMeshID)->material;
        aMaterialID = -1;
        //aShaderID = lookupUberMaterial(aMaterialID)->shader;
        aShaderID = -1;
    }

    int bMeshID;
    int bMaterialID;
    int bShaderID;
    if (IS_RENDERER_OBJECT_SIMPLE(lookupRendererObject(b->rendererObject))) {
        bMeshID = lookupRendererObject(b->rendererObject)->mesh.simpleMesh;
        bMaterialID = lookupSimpleMesh(bMeshID)->material;
        bShaderID = lookupSimpleMaterial(bMaterialID)->shader;
    } else {
        bMeshID = lookupRendererObject(b->rendererObject)->mesh.uberMesh;
        //bMaterialID = uberMaterialOffset + lookupUberMesh(bMeshID)->material;
        bMaterialID = -1;
        //bShaderID = lookupUberMaterial(bMaterialID)->shader;
        bShaderID = -1;
    }

    if (aShaderID != bShaderID) {
        return aShaderID - bShaderID;
    }
    if (aMaterialID != bMaterialID) {
        return aMaterialID - bMaterialID;
    }
    return aMeshID - bMeshID;
}

static DrawInstruction* opaqueDrawInstructions;
static void buildOpaqueDrawQueue() {
    if (opaqueDrawInstructions != NULL) {
        arrfree(opaqueDrawInstructions);
        opaqueDrawInstructions = NULL;
    }

    // Sorts render objects first by shader, then by material, then by mesh, culled objects placed last
    qsort(processedOpaqueRendererObjects, numOpaqueRendererObjects, sizeof(ProcessedOpaqueRendererObject), compareOpaqueRendererObjectsByMaterialThenShaderThenMesh);

    int currentShaderID = -1;
    int currentMaterialID = -1;
    int currentMeshID = -1;
    for (int i = 0; i < numOpaqueRendererObjects; ++i) {
        const ProcessedOpaqueRendererObject* processedOpaqueRenderObject = &processedOpaqueRendererObjects[i];
        if (processedOpaqueRenderObject->rendererObject == -1) break; // All remaining objects are culled since culled objects are always last

        int meshID;
        int materialID;
        int shaderID;
        if (IS_RENDERER_OBJECT_SIMPLE(lookupRendererObject(processedOpaqueRenderObject->rendererObject))) {
            meshID = lookupRendererObject(processedOpaqueRenderObject->rendererObject)->mesh.simpleMesh;
            materialID = lookupSimpleMesh(meshID)->material;
            shaderID = lookupSimpleMaterial(materialID)->shader;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction = { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            if (materialID != currentMaterialID) {
                const DrawInstruction bindSimpleMaterialDrawInstruction = { .type = DRAW_BIND_SIMPLE_MATERIAL, .data.simpleMaterial = materialID };
                arrput(opaqueDrawInstructions, bindSimpleMaterialDrawInstruction);
                currentMaterialID = materialID;
            }
            if (meshID != currentMeshID) {
                const DrawInstruction bindSimpleMeshDrawInstruction = { .type = DRAW_BIND_SIMPLE_MESH, .data.simpleMesh = meshID };
                arrput(opaqueDrawInstructions, bindSimpleMeshDrawInstruction);
                currentMeshID = meshID;
            }

            const DrawInstruction bindTransformInstruction = { .type = DRAW_BIND_TRANSFORM, .data.rendererObject = processedOpaqueRenderObject->rendererObject };
            arrput(opaqueDrawInstructions, bindTransformInstruction);

            const DrawInstruction drawSimpleMeshInstruction = { .type = DRAW_SIMPLE_MESH, .data.simpleMesh = meshID };
            arrput(opaqueDrawInstructions, drawSimpleMeshInstruction);
        } else {
            const int uberMeshId = lookupRendererObject(processedOpaqueRenderObject->rendererObject)->mesh.uberMesh;
            meshID = uberMeshOffset + uberMeshId;
            //materialID = uberMaterialOffset + lookupUberMesh(meshID)->material;
            materialID = -1;
            //shaderID = lookupUberMaterial(materialID)->shader;
            shaderID = -1;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction = { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            // if (materialID != currentMaterialID) {
            //     int uberMaterialID = materialID - uberMaterialOffset;
            //     const DrawInstruction bindUberMaterialDrawInstruction = { .type = DRAW_BIND_UBER_MATERIAL, .data.uberMaterial = uberMaterialID };
            //     arrput(opaqueDrawInstructions, bindUberMaterialDrawInstruction);
            //     currentMaterialID = materialID;
            // }
            // if (meshID != currentMeshID) {
            //     const DrawInstruction bindUberMeshDrawInstruction = { .type = DRAW_BIND_UBER_MESH, .data.uberMesh = uberMeshId };
            //     arrput(opaqueDrawInstructions, bindUberMeshDrawInstruction);
            //     currentMeshID = meshID;
            // }

            const DrawInstruction bindTransformInstruction = { .type = DRAW_BIND_TRANSFORM, .data.rendererObject = processedOpaqueRenderObject->rendererObject };
            arrput(opaqueDrawInstructions, bindTransformInstruction);

            // const DrawInstruction drawUberMeshInstruction = { .type = DRAW_UBER_MESH, .data.uberMesh = uberMeshId };
            // arrput(opaqueDrawInstructions, drawUberMeshInstruction);
        }
    }
}

static int compareTransparentRendererObjectsByDistanceSqr(const ProcessedTransparentRendererObject* a, const ProcessedTransparentRendererObject* b) {
    if (a->distanceSqr < b->distanceSqr) {
        return 1;
    } else {
        return -1;
    }
}

static DrawInstruction* transparentDrawInstructions;
static void buildTransparentsDrawQueue() {
    if (transparentDrawInstructions != NULL) {
        arrfree(transparentDrawInstructions);
        transparentDrawInstructions = NULL;
    }

    // Sorts render objects furthest to closest, culled objects placed last
    qsort(processedTransparentRendererObjects, numTransparentRendererObjects, sizeof(ProcessedTransparentRendererObject), compareTransparentRendererObjectsByDistanceSqr);

    int currentShaderID = -1;
    int currentMaterialID = -1;
    int currentMeshID = -1;
    for (int i = 0; i < numTransparentRendererObjects; ++i) {
        const ProcessedTransparentRendererObject* processedTransparentRenderObject = &processedTransparentRendererObjects[i];
        if (processedTransparentRenderObject->rendererObject == -1) break; // All remaining objects are culled since culled objects are always last

        int meshID;
        int materialID;
        int shaderID;
        if (IS_RENDERER_OBJECT_SIMPLE(lookupRendererObject(processedTransparentRenderObject->rendererObject))) {
            meshID = lookupRendererObject(processedTransparentRenderObject->rendererObject)->mesh.simpleMesh;
            materialID = lookupSimpleMesh(meshID)->material;
            shaderID = lookupSimpleMaterial(materialID)->shader;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction = { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            if (materialID != currentMaterialID) {
                const DrawInstruction bindSimpleMaterialDrawInstruction = { .type = DRAW_BIND_SIMPLE_MATERIAL, .data.simpleMaterial = materialID };
                arrput(opaqueDrawInstructions, bindSimpleMaterialDrawInstruction);
                currentMaterialID = materialID;
            }
            if (meshID != currentMeshID) {
                const DrawInstruction bindSimpleMeshDrawInstruction = { .type = DRAW_BIND_SIMPLE_MESH, .data.simpleMesh = meshID };
                arrput(opaqueDrawInstructions, bindSimpleMeshDrawInstruction);
                currentMeshID = meshID;
            }

            const DrawInstruction bindTransformInstruction = { .type = DRAW_BIND_TRANSFORM, .data.rendererObject = processedTransparentRenderObject->rendererObject };
            arrput(opaqueDrawInstructions, bindTransformInstruction);

            const DrawInstruction drawSimpleMeshInstruction = { .type = DRAW_SIMPLE_MESH, .data.simpleMesh = meshID };
            arrput(opaqueDrawInstructions, drawSimpleMeshInstruction);
        } else {
            const int uberMeshId = lookupRendererObject(processedTransparentRenderObject->rendererObject)->mesh.uberMesh;
            meshID = uberMeshOffset + uberMeshId;
            //materialID = uberMaterialOffset + lookupUberMesh(meshID)->material;
            materialID = -1;
            //shaderID = lookupUberMaterial(materialID)->shader;
            shaderID = -1;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction = { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            // if (materialID != currentMaterialID) {
            //     int uberMaterialID = materialID - uberMaterialOffset;
            //     const DrawInstruction bindUberMaterialDrawInstruction = { .type = DRAW_BIND_UBER_MATERIAL, .data.uberMaterial = uberMaterialID };
            //     arrput(opaqueDrawInstructions, bindUberMaterialDrawInstruction);
            //     currentMaterialID = materialID;
            // }
            // if (meshID != currentMeshID) {
            //     const DrawInstruction bindUberMeshDrawInstruction = { .type = DRAW_BIND_UBER_MESH, .data.uberMesh = uberMeshId };
            //     arrput(opaqueDrawInstructions, bindUberMeshDrawInstruction);
            //     currentMeshID = meshID;
            // }

            const DrawInstruction bindTransformInstruction = { .type = DRAW_BIND_TRANSFORM, .data.rendererObject = processedTransparentRenderObject->rendererObject };
            arrput(opaqueDrawInstructions, bindTransformInstruction);

            // const DrawInstruction drawUberMeshInstruction = { .type = DRAW_UBER_MESH, .data.uberMesh = uberMeshId };
            // arrput(opaqueDrawInstructions, drawUberMeshInstruction);
        }
    }
}

static void mergeDrawQueues() {
    lockJobTree(&renderTree);
    clearJobDynamicDeps(buildOpaqueDrawQueueJob);
    clearJobDynamicDeps(buildTransparentsDrawQueueJob);
    unlockJobTree(&renderTree);

    if (drawInstructions != NULL) {
        arrfree(drawInstructions);
        drawInstructions = NULL;
    }

    const int numOpaqueDrawInstructions = arrlen(opaqueDrawInstructions);
    const int numTransparentDrawInstructions = arrlen(transparentDrawInstructions);
    arraddnptr(drawInstructions, numOpaqueDrawInstructions + numTransparentDrawInstructions);

    memcpy(drawInstructions, opaqueDrawInstructions, sizeof(DrawInstruction) * numOpaqueDrawInstructions);
    memcpy(drawInstructions + sizeof(DrawInstruction) * numOpaqueDrawInstructions, transparentDrawInstructions, sizeof(DrawInstruction) * numTransparentDrawInstructions);

    arrfree(opaqueDrawInstructions);
    opaqueDrawInstructions = NULL;

    arrfree(transparentDrawInstructions);
    transparentDrawInstructions = NULL;
}

void initRenderer() {
    processedOpaqueRendererObjects = NULL;
    processedTransparentRendererObjects = NULL;

    opaqueDrawInstructions = NULL;
    transparentDrawInstructions = NULL;

    initRendererObjects();

    Job iterateObjectsJob;
    {
        initJob(&iterateObjectsJob, 0, NULL, iterateVisibleObjects, (JobData){ NULL }, "Iterate Objects");
    }

    Job buildOpaqueDrawQueueJobLcl;
    {
        Job* buildOpaqueDrawQueueDeps = malloc(1 * sizeof(Job));
        buildOpaqueDrawQueueDeps[0] = iterateObjectsJob;

        initJob(&buildOpaqueDrawQueueJobLcl, 1, buildOpaqueDrawQueueDeps, buildOpaqueDrawQueue, (JobData){ NULL }, "Build Opaque Draw Queue");
    }

    Job buildTransparentsDrawQueueJobLcl;
    {
        Job* buildTransparentsDrawQueueDeps = malloc(1 * sizeof(Job));
        buildTransparentsDrawQueueDeps[0] = iterateObjectsJob;

        initJob(&buildTransparentsDrawQueueJobLcl, 1, buildTransparentsDrawQueueDeps, buildTransparentsDrawQueue, (JobData){ NULL }, "Build Transparents Draw Queue");
    }

    Job mergeDrawQueuesJob;
    {
        Job* mergeDrawQueuesDeps = malloc(2 * sizeof(Job));
        mergeDrawQueuesDeps[0] = buildOpaqueDrawQueueJobLcl;
        mergeDrawQueuesDeps[1] = buildTransparentsDrawQueueJobLcl;

        buildOpaqueDrawQueueJob = &mergeDrawQueuesDeps[0];
        buildTransparentsDrawQueueJob = &mergeDrawQueuesDeps[1];

        initJob(&mergeDrawQueuesJob, 2, mergeDrawQueuesDeps , mergeDrawQueues, (JobData){ NULL }, "Merge Draw Queues");
    }

    Job* renderTreeJobs = malloc(1 * sizeof(Job));
    renderTreeJobs[0] = mergeDrawQueuesJob;

    initJobTree(&renderTree, 1, renderTreeJobs, "Renderer");
}

void freeRenderer() {
    freeRendererObjects();
    freeJobTree(&renderTree);
}

void executeRenderTreeAsync() {
    clearJobDynamicDeps(buildOpaqueDrawQueueJob);
    clearJobDynamicDeps(buildTransparentsDrawQueueJob);

    resetJobTree(&renderTree);
    executeJobTreeAsync(&renderTree);
}