#pragma once
#ifndef __BFS_HPP
#define __BFS_HPP

#include <queue>
#include <vector>
#include <algorithm>
#include "../GridPlanner.h"
using std::queue;
using std::vector;
vector<coor> dirs = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

class bfs {
public:
    bfs(vector<vector<gState>> _map, coor _start, coor _end);
    ~bfs();

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
};

bfs::bfs(vector<vector<gState>> _map, coor _start, coor _end) 
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

bfs::~bfs()
{

}

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
    
    coor tmp = end;
    while (tmp != start) {
        path.emplace_back(tmp);
        tmp = parent[tmp.y][tmp.x];
    }
    path.emplace_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}

bool bfs::checkCoor(int x, int y)
{
    return (x >= 0 && x < col && y >= 0 && y < row && 
        (map[y][x] == IDLE || map[y][x] == TARGET)); 
}

#endif

