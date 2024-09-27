#include "src/engine/engine.h"
#include "src/engine/window.h"

int main(void)
{
    initEngine();
    engineLoop();
    freeEngine();

    return 0;
}
