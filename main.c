#include "src/jobs/job_system.h"
#include "src/engine/engine.h"
#include "src/engine/assets/shader.h"
#include "src/engine/assets/simple_material.h"
#include "src/engine/assets/simple_mesh.h"
#include "src/engine/utils/model_loader.h"
#include "src/renderer/renderer_object.h"

int main(void)
{
    initJobSystem(1);

    initEngine();

    // Testing

    const ShaderID simpleShader = createShaderFromPaths(SHADER_SIMPLE);
    if (simpleShader == -1) {
        return 1;
    }

    const SimpleMaterialID simpleMaterial = createSimpleMaterial(simpleShader, (vec4){136.0f / 255.0f, 82.0f / 255.0f, 127.0f / 255.0f, 1.0f});
    if (simpleMaterial == -1) {
        return 2;
    }

    SimpleModel simpleModel;
    loadSimpleModel(ASSETS_DIR "shapes" DIR "cube.obj", simpleMaterial, &simpleModel);

    const float xSpace = 1.5f;
    const float zSpace = 1.5f;

    const int numX = 10;
    const int numZ = 10;

    for (int x = 0; x < numX; ++x) {
        for (int z = 0; z < numZ; ++z) {
            const RendererObjectID rendererObject = registerSimpleRendererObject(simpleModel.meshes[0], false);
            glm_translate_make(lookupRendererObject(rendererObject)->transform.matrix, (vec3){((float)x - (float)numX / 2.0f + 0.5f) * xSpace, 0.0f, ((float)z - (float)numZ / 2.0f + 0.5f) * zSpace});
        }
    }

    //

    engineLoop();
    freeEngine();

    freeJobSystem();

    return 0;
}
