#include "BFS.hpp"
#include <algorithm>
using std::max;

int main()
{
    vector<vector<gState>> map;
    GridPlanner* gp = new GridPlanner();
    // bfs* search;
    vector<bfs*> search(3);
    vector<vector<coor>> path(3);
    vector<coor> agents  = {{0, 0}, {0, 19}, {16, 19}};
    vector<coor> targets = {{11, 6}, {12, 12}, {20, 4}};
    gp->loadGridMap("../../maps/maze");
    gp->addGridAgents(3, agents[0],agents[1],agents[2]);
    gp->addGridTargets(3, targets[0],targets[1],targets[2]);
    // gp->addGridAgent(0, agents[0]);
    // gp->addGridAgent(1, agents[1]);
    // gp->addGridAgent(2, agents[2]);
    // gp->addGridTarget(0, targets[0]);
    // gp->addGridTarget(1, targets[1]);
    // gp->addGridTarget(2, targets[2]);
    
    map = gp->getGridMap();
    // search = new bfs(map, {0, 0}, {14, 0}, true);
    // search = new bfs(map, {0, 0}, {14, 0});
    // path = search->run();
    search[0] = new bfs(map, agents[0], targets[0]);
    search[1] = new bfs(map, agents[1], targets[1]);
    search[2] = new bfs(map, agents[2], targets[2]);
    path[0] = search[0]->run();
    path[1] = search[1]->run();
    path[2] = search[2]->run();
    gp->run();
    SDL_Delay(3000);
    int maxLen = max(max(path[0].size(), path[1].size()), path[2].size());
    for (int i = 0; i < maxLen; ++i) {
        if (i < path[0].size()) {
            gp->setAgentPos(0, path[0][i]);
        }
        if (i < path[1].size()) {
            gp->setAgentPos(1, path[1][i]);
        }
        if (i < path[2].size()) {
            gp->setAgentPos(2, path[2][i]);
        }


        SDL_Delay(300);
        if (gp->getGridShouldQuit()) {
            break;
        }
    }

    gp->addShowPath(path[0], pLINE, GREEN);
    gp->addShowPath(path[1], pLINE, ORANGE);
    gp->addShowPath(path[2], pLINE, MAGENTA);

    while(!gp->getGridShouldQuit()) {
        // do something;
    }

    delete search[0];
    delete search[1];
    delete search[2];
    delete gp;
    return 0;
}