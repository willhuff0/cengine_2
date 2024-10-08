cmake_minimum_required(VERSION 3.29)
project(cengine_2 C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(cengine_2 main.c
        src/jobs/job.c
        src/jobs/job.h
        src/common.h
        src/jobs/job_system.c
        src/jobs/job_system.h
        src/jobs/job_worker.c
        src/jobs/job_worker.h
        src/stb_impl.c
        src/stbi_impl.c
        src/renderer/frame_packet.c
        src/renderer/frame_packet.h
        src/simulation/sim_object.c
        src/simulation/sim_object.h
        src/engine/window.c
        src/engine/window.h
        src/common.c
        src/engine/engine.c
        src/engine/engine.h
        src/simulation/sim.c
        src/simulation/sim.h
        src/engine/input.c
        src/engine/input.h
        src/simulation/network/network.c
        src/simulation/network/network.h
        src/simulation/physics/physics.c
        src/simulation/physics/physics.h
        src/simulation/logic/logic.c
        src/simulation/logic/logic.h
        src/renderer/renderer.c
        src/renderer/renderer.h
        src/simulation/frame_packet_generator.c
        src/simulation/frame_packet_generator.h
        src/engine/assets.c
        src/engine/assets.h
        src/engine/assets/texture.c
        src/engine/assets/texture.h
        src/engine/assets/shader.c
        src/engine/assets/shader.h
        src/engine/assets/simple_material.c
        src/engine/assets/simple_material.h
        src/engine/assets/simple_mesh.c
        src/engine/assets/simple_mesh.h
        src/engine/bounding_box.c
        src/engine/bounding_box.h
        src/renderer/renderer_object.c
        src/renderer/renderer_object.h
        src/engine/draw_system.c
        src/engine/draw_system.h
        src/engine/utils/fps_counter.c
        src/engine/utils/fps_counter.h
        src/paths.h
        src/engine/utils/model_loader.c
        src/engine/utils/model_loader.h
)

target_include_directories(cengine_2 PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/angle)

target_link_libraries(cengine_2 PRIVATE
        ${PROJECT_SOURCE_DIR}/libs/libglfw3.a
        ${PROJECT_SOURCE_DIR}/libs/libGLESv2.dll.lib
        ${PROJECT_SOURCE_DIR}/libs/libEGL.dll.lib
        ${PROJECT_SOURCE_DIR}/libs/libcglm.a
        ${PROJECT_SOURCE_DIR}/libs/assimp-vc143-mt.lib
)
