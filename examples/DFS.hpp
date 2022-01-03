#pragma once
#ifndef __DFS_HPP
#define __DFS_HPP

#include <vector>
#include <algorithm>
#include "../GridPlanner.h"

using std::vector;
vector<coor> dirs = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

class dfs {
public:
    dfs(vector<vector<gState>> _map, coor _start, coor _end);
    ~dfs();

    vector<coor> run(void);
    bool checkCoor(int x, int y);

private:
    int row;
    int col;
    coor end;
    coor start;
    vector<vector<gState>> map;
    vector<vector<bool>>   visited;
    vector<vector<coor>>   parent;

    bool searchPath(coor curNode);
};

dfs::dfs(vector<vector<gState>> _map, coor _start, coor _end) 
{
    row = _map.size();
    col = _map[0].size();
    map = _map;
    start = _start;
    end = _end;
    visited.resize(row);
    parent.resize(row);
    for (int i = 0; i < row; ++i) {
        visited[i].resize(col);
        parent[i].resize(col);
    }
}

dfs::~dfs()
{

}

vector<coor> dfs::run(void)
{
    vector<coor> path;
    
    if (!searchPath(start)) {
        return path;
    }

    coor tmp = end;
    while (tmp != start) {
        path.emplace_back(tmp);
        tmp = parent[tmp.y][tmp.x];
    }
    path.emplace_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}

bool dfs::checkCoor(int x, int y)
{
    return (x >= 0 && x < col && y >= 0 && y < row && 
        (map[y][x] == IDLE || map[y][x] == TARGET)); 
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

