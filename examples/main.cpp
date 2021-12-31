#include <iostream>
#include "../GridPlanner.h"
#include <unistd.h>

int main( int argc, char* args[] )
{
    GridPlanner* gp = new GridPlanner(480,480,12,12);
    // gp->setShowGridLine(false);
    // gp->createGridMap(630,630,21,21,"/home/puyu/CODE/GridPlanning/maze");
    // gp->loadGridMap(0.2);
    gp->loadGridMap("/home/puyu/CODE/GridPlanner/maps/cross");
    gp->addGridAgent(0, 0, 0);
    gp->addGridTarget(0, 9, 9);
    gp->run();

    while(1) {
        ;
    }

    delete gp;
    return 0;
}
