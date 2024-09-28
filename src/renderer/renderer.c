//
// Created by wehuf on 9/26/2024.
//

#include "renderer.h"

#include "frame_packet.h"
#include "renderer_object.h"
#include "../engine/draw_system.h"

Job renderTreeExit;

static Job iterateObjectsJob;
static Job buildOpaqueDrawQueueJob;
static Job buildTransparentDrawQueueJob;

static typedef struct {
    RendererObjectID rendererObject;
    int processedRenderObjectIndex;
} ProcessRendererObjectInstruction;

static typedef struct {
    RendererObject* rendererObject;
} ProcessedOpaqueRendererObject;

static typedef struct {
    RendererObject* rendererObject;
    float distanceSqr;
} ProcessedTransparentRendererObject;

static int numOpaqueRendererObjects;
static ProcessedOpaqueRendererObject* processedOpaqueRendererObjects;

static int numTransparentRendererObjects;
static ProcessedTransparentRendererObject* processedTransparentRendererObjects;

static int uberMeshOffset;
static int uberMaterialOffset;

static void processOpaqueRenderObject(const ProcessRendererObjectInstruction instruction) {
    if (!aabbIsInFrustum(GET_RENDERER_OBJECT_AABB(lookupRendererObject(instruction.rendererObject)), lookupRendererObject(instruction.rendererObject)->transform.matrix, rendererFramePacket->view.frustum)) {
        processedOpaqueRendererObjects[instruction.processedRenderObjectIndex].rendererObject = NULL;
        return;
    }

    processedOpaqueRendererObjects[instruction.processedRenderObjectIndex].rendererObject = lookupRendererObject(instruction.rendererObject);
}

static void processTransparentRenderObject(const ProcessRendererObjectInstruction instruction) {
    if (!aabbIsInFrustum(GET_RENDERER_OBJECT_AABB(lookupRendererObject(instruction.rendererObject)), lookupRendererObject(instruction.rendererObject)->transform.matrix, rendererFramePacket->view.frustum)) {
        processedTransparentRendererObjects[instruction.processedRenderObjectIndex].rendererObject = NULL;
        processedTransparentRendererObjects[instruction.processedRenderObjectIndex].distanceSqr = -1.0f;
        return;
    }

    const float distanceSqr = glm_vec3_distance2(rendererFramePacket->view.pos, lookupRendererObject(instruction.rendererObject)->transform.matrix);

    processedTransparentRendererObjects[instruction.processedRenderObjectIndex].rendererObject = lookupRendererObject(instruction.rendererObject);
    processedTransparentRendererObjects[instruction.processedRenderObjectIndex].distanceSqr = distanceSqr;
}

// Creates jobs (deps for sort transparents and build opaque draw queue)
static void iterateVisibleObjects() {
    // Prevent workers from starting jobs before the buffers are allocated
    pthread_mutex_lock(buildOpaqueDrawQueueJob.mutex);
    pthread_mutex_lock(buildTransparentDrawQueueJob.mutex);

    numOpaqueRendererObjects = 0;
    numTransparentRendererObjects = 0;

    // Create jobs and count objects
    for (int i = 0; i < arrlen(rendererObjects); ++i) {
        switch (rendererObjects[i].type) {
            case SIMPLE_OPAQUE_RENDERER_OBJECT:
            case UBER_OPAQUE_RENDERER_OBJECT:
                Job processOpaqueRenderObjectJob;
                initJob(&processOpaqueRenderObjectJob, NULL, processOpaqueRenderObject, { .twoNumbers = { i, numOpaqueRendererObjects } }, "[Renderer] Process Opaque Renderer Object");

                arrput(buildOpaqueDrawQueueJob.deps, processOpaqueRenderObjectJob);

                numOpaqueRendererObjects++;
                break;
            case SIMPLE_TRANSPARENT_RENDERER_OBJECT:
            case UBER_TRANSPARENT_RENDERER_OBJECT:
                Job processTransparentRenderObjectJob;
                initJob(&processTransparentRenderObjectJob, NULL, processTransparentRenderObject, { .twoNumbers = { i, numTransparentRendererObjects } }, "[Renderer] Process Transparent Renderer Object");

                arrput(buildTransparentDrawQueueJob.deps, processTransparentRenderObjectJob);

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
    pthread_mutex_unlock(buildOpaqueDrawQueueJob.mutex);
    pthread_mutex_unlock(buildTransparentDrawQueueJob.mutex);
}

static int compareOpaqueRendererObjectsByMaterialThenShaderThenMesh(const ProcessedOpaqueRendererObject* a, const ProcessedOpaqueRendererObject* b) {
    int aMeshID;
    int aMaterialID;
    int aShaderID;
    if (IS_RENDERER_OBJECT_SIMPLE(a->rendererObject)) {
        aMeshID = a->rendererObject->mesh.simpleMesh;
        aMaterialID = lookupSimpleMesh(aMeshID)->material;
        aShaderID = lookupSimpleMaterial(aMaterialID)->shader;
    } else {
        aMeshID = a->rendererObject->mesh.uberMesh;
        //aMaterialID = uberMaterialOffset + lookupUberMesh(aMeshID)->material;
        aMaterialID = -1;
        //aShaderID = lookupUberMaterial(aMaterialID)->shader;
        aShaderID = -1;
    }

    int bMeshID;
    int bMaterialID;
    int bShaderID;
    if (IS_RENDERER_OBJECT_SIMPLE(b->rendererObject)) {
        bMeshID = b->rendererObject->mesh.simpleMesh;
        bMaterialID = lookupSimpleMesh(bMeshID)->material;
        bShaderID = lookupSimpleMaterial(bMaterialID)->shader;
    } else {
        bMeshID = b->rendererObject->mesh.uberMesh;
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

    // Sorts render objects first by shader, then by material, then by mesh
    qsort(processedOpaqueRendererObjects, numOpaqueRendererObjects, sizeof(ProcessedOpaqueRendererObject), compareOpaqueRendererObjectsByMaterialThenShaderThenMesh);

    int currentShaderID = -1;
    int currentMaterialID = -1;
    int currentMeshID = -1;
    for (int i = 0; i < numOpaqueRendererObjects; ++i) {
        const ProcessedOpaqueRendererObject* processedOpaqueRenderObject = &processedOpaqueRendererObjects[i];

        int meshID;
        int materialID;
        int shaderID;
        if (IS_RENDERER_OBJECT_SIMPLE(processedOpaqueRenderObject->rendererObject)) {
            meshID = processedOpaqueRenderObject->rendererObject->mesh.simpleMesh;
            materialID = lookupSimpleMesh(meshID)->material;
            shaderID = lookupSimpleMaterial(materialID)->shader;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            if (materialID != currentMaterialID) {
                const DrawInstruction bindSimpleMaterialDrawInstruction { .type = DRAW_BIND_SIMPLE_MATERIAL, .data.simpleMaterial = materialID };
                arrput(opaqueDrawInstructions, bindSimpleMaterialDrawInstruction);
                currentMaterialID = materialID;
            }
            if (meshID != currentMeshID) {
                const DrawInstruction bindSimpleMeshDrawInstruction { .type = DRAW_BIND_SIMPLE_MESH, .data.simpleMesh = meshID };
                arrput(opaqueDrawInstructions, bindSimpleMeshDrawInstruction);
                currentMeshID = meshID;
            }

            const DrawInstruction drawSimpleMeshInstruction { .type = DRAW_SIMPLE_MESH, .data.simpleMesh = meshID };
            arrput(opaqueDrawInstructions, drawSimpleMeshInstruction);
        } else {
            const int uberMeshId = processedOpaqueRenderObject->rendererObject->mesh.uberMesh;
            meshID = uberMeshOffset + uberMeshId;
            //materialID = uberMaterialOffset + lookupUberMesh(meshID)->material;
            materialID = -1;
            //shaderID = lookupUberMaterial(materialID)->shader;
            shaderID = -1;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            // if (materialID != currentMaterialID) {
            //     int uberMaterialID = materialID - uberMaterialOffset;
            //     const DrawInstruction bindUberMaterialDrawInstruction { .type = DRAW_BIND_UBER_MATERIAL, .data.uberMaterial = uberMaterialID };
            //     arrput(opaqueDrawInstructions, bindUberMaterialDrawInstruction);
            //     currentMaterialID = materialID;
            // }
            // if (meshID != currentMeshID) {
            //     const DrawInstruction bindUberMeshDrawInstruction { .type = DRAW_BIND_UBER_MESH, .data.uberMesh = uberMeshId };
            //     arrput(opaqueDrawInstructions, bindUberMeshDrawInstruction);
            //     currentMeshID = meshID;
            // }

            // const DrawInstruction drawUberMeshInstruction { .type = DRAW_UBER_MESH, .data.uberMesh = uberMeshId };
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

        int meshID;
        int materialID;
        int shaderID;
        if (IS_RENDERER_OBJECT_SIMPLE(processedTransparentRenderObject->rendererObject)) {
            meshID = processedTransparentRenderObject->rendererObject->mesh.simpleMesh;
            materialID = lookupSimpleMesh(meshID)->material;
            shaderID = lookupSimpleMaterial(materialID)->shader;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            if (materialID != currentMaterialID) {
                const DrawInstruction bindSimpleMaterialDrawInstruction { .type = DRAW_BIND_SIMPLE_MATERIAL, .data.simpleMaterial = materialID };
                arrput(opaqueDrawInstructions, bindSimpleMaterialDrawInstruction);
                currentMaterialID = materialID;
            }
            if (meshID != currentMeshID) {
                const DrawInstruction bindSimpleMeshDrawInstruction { .type = DRAW_BIND_SIMPLE_MESH, .data.simpleMesh = meshID };
                arrput(opaqueDrawInstructions, bindSimpleMeshDrawInstruction);
                currentMeshID = meshID;
            }

            const DrawInstruction drawSimpleMeshInstruction { .type = DRAW_SIMPLE_MESH, .data.simpleMesh = meshID };
            arrput(opaqueDrawInstructions, drawSimpleMeshInstruction);
        } else {
            const int uberMeshId = processedTransparentRenderObject->rendererObject->mesh.uberMesh;
            meshID = uberMeshOffset + uberMeshId;
            //materialID = uberMaterialOffset + lookupUberMesh(meshID)->material;
            materialID = -1;
            //shaderID = lookupUberMaterial(materialID)->shader;
            shaderID = -1;

            if (shaderID != currentShaderID) {
                const DrawInstruction bindShaderDrawInstruction { .type = DRAW_BIND_SHADER, .data.shader = shaderID };
                arrput(opaqueDrawInstructions, bindShaderDrawInstruction);
                currentShaderID = shaderID;
            }
            // if (materialID != currentMaterialID) {
            //     int uberMaterialID = materialID - uberMaterialOffset;
            //     const DrawInstruction bindUberMaterialDrawInstruction { .type = DRAW_BIND_UBER_MATERIAL, .data.uberMaterial = uberMaterialID };
            //     arrput(opaqueDrawInstructions, bindUberMaterialDrawInstruction);
            //     currentMaterialID = materialID;
            // }
            // if (meshID != currentMeshID) {
            //     const DrawInstruction bindUberMeshDrawInstruction { .type = DRAW_BIND_UBER_MESH, .data.uberMesh = uberMeshId };
            //     arrput(opaqueDrawInstructions, bindUberMeshDrawInstruction);
            //     currentMeshID = meshID;
            // }

            // const DrawInstruction drawUberMeshInstruction { .type = DRAW_UBER_MESH, .data.uberMesh = uberMeshId };
            // arrput(opaqueDrawInstructions, drawUberMeshInstruction);
        }
    }
}

static void mergeDrawQueues() {
    if (arrlen(buildOpaqueDrawQueueJob.deps) > 1) {
        for (int i = 1; i < arrlen(buildOpaqueDrawQueueJob.deps); ++i) {
            freeJobTree(&buildOpaqueDrawQueueJob.deps[i]);
        }
        arrfree(buildOpaqueDrawQueueJob.deps);
        buildOpaqueDrawQueueJob.deps = NULL;
        arrput(buildOpaqueDrawQueueJob.deps, iterateObjectsJob);
    }

    if (arrlen(buildTransparentDrawQueueJob.deps) > 1 ) {
        for (int i = 1; i < arrlen(buildTransparentDrawQueueJob.deps); ++i) {
            freeJobTree(&buildTransparentDrawQueueJob.deps[i]);
        }
        arrfree(buildTransparentDrawQueueJob.deps);
        buildTransparentDrawQueueJob.deps = NULL;
        arrput(buildTransparentDrawQueueJob.deps, iterateObjectsJob);
    }

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

    {
        initJob(&iterateObjectsJob, NULL, iterateVisibleObjects, { NULL }, "[RENDERER] Iterate Objects");
    }

    {
        Job* buildOpaqueDrawQueueDeps = NULL;
        arrput(buildOpaqueDrawQueueDeps, iterateObjectsJob);

        initJob(&buildOpaqueDrawQueueJob, buildOpaqueDrawQueueDeps, buildOpaqueDrawQueue, { NULL }, "[RENDERER] Build Opaque Draw Queue");
    }

    Job buildTransparentsDrawQueueJob;
    {
        Job* buildTransparentsDrawQueueDeps = NULL;
        arrput(buildTransparentsDrawQueueDeps, iterateObjectsJob);

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
    freeRendererObjects();
    freeJobTree(&renderTreeExit);
}

void executeRenderTreeAsync() {
    if (arrlen(buildOpaqueDrawQueueJob.deps) > 1) {
        for (int i = 1; i < arrlen(buildOpaqueDrawQueueJob.deps); ++i) {
            freeJobTree(&buildOpaqueDrawQueueJob.deps[i]);
        }
        arrfree(buildOpaqueDrawQueueJob.deps);
        buildOpaqueDrawQueueJob.deps = NULL;
        arrput(buildOpaqueDrawQueueJob.deps, iterateObjectsJob);
    }

    if (arrlen(buildTransparentDrawQueueJob.deps) > 1 ) {
        for (int i = 1; i < arrlen(buildTransparentDrawQueueJob.deps); ++i) {
            freeJobTree(&buildTransparentDrawQueueJob.deps[i]);
        }
        arrfree(buildTransparentDrawQueueJob.deps);
        buildTransparentDrawQueueJob.deps = NULL;
        arrput(buildTransparentDrawQueueJob.deps, iterateObjectsJob);
    }

    resetJobTree(&renderTreeExit);
    executeJobTreeAsync(&renderTreeExit);
}