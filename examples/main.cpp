#include <iostream>
#include "../GridPlanner.h"

int main( int argc, char* args[] )
{
    GridPlanner* gp = new GridPlanner(480,480,12,12);
    coor a1 = {0,0};
    coor a2 = {0,1};
    // gp->setShowGridLine(false);
    // gp->createGridMap(630,630,21,21,"/home/puyu/CODE/GridPlanning/maze");
    // gp->loadGridMap(0.2);
    gp->loadGridMap("../../maps/cross");
    // gp->addGridAgent(0, 0, 0);
    // gp->addGridTarget(0, 9, 9);
    gp->addGridAgents(2, a1, a2);
    gp->run();

    while(!gp->getGridShouldQuit()) {
        ;
    }

    delete gp;
    return 0;
}
