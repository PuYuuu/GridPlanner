#include "DFS.hpp"

int main()
{
    vector<vector<gState>> map;
    GridPlanner* gp = new GridPlanner();
    dfs* search;
    vector<coor> path;
    gp->loadGridMap("../../maps/maze");
    gp->addGridAgent(0, {0, 0});
    gp->addGridTarget(0, {14, 0});
    map = gp->getGridMap();
    search = new dfs(map, {0, 0}, {14, 0});
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