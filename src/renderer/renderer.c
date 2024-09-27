//
// Created by wehuf on 9/26/2024.
//

#include "renderer.h"

Job renderTreeExit;

void initRenderer() {}
void freeRenderer() {
    freeJobTree(&renderTreeExit);
}

void executeRenderTreeAsync() {
    resetJobTree(&renderTreeExit);
    executeJobTreeAsync(&renderTreeExit);
}