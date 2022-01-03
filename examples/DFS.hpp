#pragma once
#ifndef __DFS_HPP
#define __DFS_HPP

#include "pathSearch.hpp"

class dfs : public pathSearch{
public:
    dfs(vector<vector<gState>> _map, coor _start, coor _end, bool _enableDiagonal = false):
        pathSearch(_map,  _start, _end, _enableDiagonal){};
    ~dfs() {};

    vector<coor> run(void);

private:
    bool searchPath(coor curNode);
};

vector<coor> dfs::run(void)
{
    vector<coor> path;
    
    if (!searchPath(start)) {
        return path;
    }

    path = buildPath();

    return path;
}

bool dfs::searchPath(coor curNode)
{
    if (curNode == end) {
        return true;
    }

    visited[curNode.y][curNode.x] = true;
    for (auto& dir : dirs) {
        coor coorTmp;
        coorTmp.x = curNode.x + dir.x;
        coorTmp.y = curNode.y + dir.y;
        if (checkCoor(coorTmp.x, coorTmp.y) && !visited[coorTmp.y][coorTmp.x]) {
            parent[coorTmp.y][coorTmp.x] = curNode;
            if (searchPath(coorTmp)) {
                return true;
            }
        }
    }

    return false;
}

#endif

