#pragma once
#ifndef __BFS_HPP
#define __BFS_HPP

#include <queue>
#include "pathSearch.hpp"
using std::queue;

class bfs : public pathSearch{
public:
    bfs(vector<vector<gState>> _map, coor _start, coor _end, bool _enableDiagonal = false) : 
        pathSearch(_map,  _start, _end, _enableDiagonal){};
    ~bfs() {};

    vector<coor> run(void);

private:
    
};

vector<coor> bfs::run(void)
{
    vector<coor> path;
    queue<coor> q;
    q.emplace(start);

    if (map[end.y][end.x] == OCCUPY_O) {
        return path;
    }

    while (!q.empty()) {
        coor tmp = q.front();
        q.pop();
        if (tmp == end) {
            break;
        }

        for (auto& dir : dirs) {
            int x = tmp.x + dir.x;
            int y = tmp.y + dir.y;
            if (checkCoor(x, y) && !visited[y][x]) {
                coor xyTmp;
                visited[y][x] = true;
                parent[y][x] = tmp;
                xyTmp.x = x, xyTmp.y = y;
                q.emplace(xyTmp);
            }
        }
    }
    
    path = buildPath();

    return path;
}

#endif

