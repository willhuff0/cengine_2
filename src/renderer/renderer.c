//
// Created by wehuf on 9/26/2024.
//

#include "renderer.h"

#include <stdio.h>
#include <unistd.h>

Job renderTreeExit;

void initRenderer() {}
void freeRenderer() {}

void executeRenderTreeAsync() {
    printf("executeRenderTree start\n");
    sleep(3);
    printf("executeRenderTree done\n");
}