#include "src/jobs/job_system.h"
#include "src/engine/engine.h"

int main(void)
{
    initJobSystem(1);

    initEngine();
    engineLoop();
    freeEngine();

    freeJobSystem();

    return 0;
}
