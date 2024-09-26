cmake_minimum_required(VERSION 3.29)
project(cengine_2 C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(cengine_2 main.c
        "src/jobs/job.c"
        "src/jobs/job.h"
        src/common.h
        src/jobs/job_system.c
        src/jobs/job_system.h
        src/jobs/job_worker.c
        src/jobs/job_worker.h
        src/stb_impl.c
        src/stbi_impl.c)

target_include_directories(cengine_2 PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/angle)

target_link_libraries(cengine_2 PRIVATE
        ${PROJECT_SOURCE_DIR}/libs/libglfw3.a
        ${PROJECT_SOURCE_DIR}/libs/libGLESv2.lib
        ${PROJECT_SOURCE_DIR}/libs/libEGL.lib
        ${PROJECT_SOURCE_DIR}/libs/libassimp.dll.a
        ${PROJECT_SOURCE_DIR}/libs/libcglm.a
        ${PROJECT_SOURCE_DIR}/libs/libode_single.dll.a
)
