#include "BFS.hpp"

int main()
{
    vector<vector<gState>> map;
    GridPlanner* gp = new GridPlanner();
    bfs* search;
    vector<coor> path;
    // gp->createGridMap(480,480,12,12,"/home/puyu/CODE/GridPlanner/maps/empty12*12");
    gp->loadGridMap("/home/puyu/CODE/GridPlanner/maps/maze");
    gp->addGridAgent(0, {0, 0});
    gp->addGridTarget(0, {14, 0});
    map = gp->getGridMap();
    search = new bfs(map, {0, 0}, {14, 0}, true);
    path = search->run();
    gp->run();

    for (coor& p : path) {
        gp->setAgentPos(0, p);
        SDL_Delay(500);
        if (gp->getGridShouldQuit()) {
            break;
        }
    }

    while(!gp->getGridShouldQuit()) {
        // do something;
    }

    delete search;
    delete gp;
    return 0;
}