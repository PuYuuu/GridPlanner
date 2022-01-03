#pragma once
#ifndef __PATHSEARCH_H
#define __PATHSEARCH_H

#include <vector>
#include <algorithm>
#include "../GridPlanner.h"
using std::vector;

class pathSearch {
public:
    pathSearch(vector<vector<gState>> _map, coor _start, coor _end, bool _enableDiagonal = false);
    ~pathSearch();

    virtual vector<coor> run(void) = 0;
    bool checkCoor(int x, int y);
    vector<coor> buildPath(void);

protected:
    int row;
    int col;
    coor end;
    coor start;
    bool enableDiagonal;
    vector<vector<gState>> map;
    vector<vector<bool>>   visited;
    vector<vector<coor>>   parent;
    vector<coor> dirs;

private:

};

pathSearch::pathSearch(vector<vector<gState>> _map, coor _start, coor _end, bool _enableDiagonal) 
{
    row = _map.size();
    col = _map[0].size();
    map = _map;
    start = _start;
    end = _end;
    enableDiagonal = _enableDiagonal;
    visited.resize(row);
    parent.resize(row);
    for (int i = 0; i < row; ++i) {
        visited[i].resize(col);
        parent[i].resize(col);
    }
    if (enableDiagonal) {
        dirs = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, 
                {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    } else {
        dirs = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    }
}

pathSearch::~pathSearch()
{

}

bool pathSearch::checkCoor(int x, int y)
{
    return (x >= 0 && x < col && y >= 0 && y < row && 
        (map[y][x] == IDLE || map[y][x] == TARGET)); 
}

vector<coor> pathSearch::buildPath(void)
{
    vector<coor> path;

    coor tmp = end;
    while (tmp != start) {
        path.emplace_back(tmp);
        tmp = parent[tmp.y][tmp.x];
    }
    path.emplace_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}

#endif

