//
// Created by wehuf on 9/26/2024.
//

#include "visibility.h"

Job visibilityTreeExit;

void initVisibility() {

}

void freeVisibility() {
    freeJobTree(&visibilityTreeExit);
}
