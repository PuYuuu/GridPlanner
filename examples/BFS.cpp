#include "BFS.hpp"

int main()
{
    vector<vector<gState>> map;
    GridPlanner* gp = new GridPlanner();
    bfs* search;
    vector<coor> path;
    gp->loadGridMap("../../maps/maze");
    gp->addGridAgent(0, {0, 0});
    gp->addGridTarget(0, {14, 0});
    map = gp->getGridMap();
    // search = new bfs(map, {0, 0}, {14, 0}, true);
    search = new bfs(map, {0, 0}, {14, 0});
    path = search->run();
    gp->run();

    for (coor& p : path) {
        gp->setAgentPos(0, p);
        SDL_Delay(300);
        if (gp->getGridShouldQuit()) {
            break;
        }
    }

    gp->addShowPath(path, pLINE, GREEN);

    while(!gp->getGridShouldQuit()) {
        // do something;
    }

    delete search;
    delete gp;
    return 0;
}