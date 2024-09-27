//
// Created by wehuf on 9/26/2024.
//

#include "renderer.h"

#include "visibility/visibility.h"

Job renderTreeExit;

void initRenderer() {
    initVisibility();
}
void freeRenderer() {
    freeVisibility();

    freeJobTree(&renderTreeExit);
}

void executeRenderTreeAsync() {
    resetJobTree(&renderTreeExit);
    executeJobTreeAsync(&renderTreeExit);
}