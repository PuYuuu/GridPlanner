#pragma once
#ifndef __GRIDPLANNING_H
#define __GRIDPLANNING_H

#include <mutex>
#include <vector>
#include <string>
#include <thread>
#include <unordered_map>
#include <SDL2/SDL.h>

using std::vector;
using std::string;
using std::pair;
using std::unordered_map;

enum gColor  { WHITE, BLACK, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, GRAY, BROWN };
enum gState  { IDLE, OCCUPY_O, OCCUPY_A, TARGET };
enum gAction { UP, DOWN, LEFT, RIGHT, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT, STOP };
enum gPathType { pLINE, pGRID };

struct GridAgent {
    uint16_t Id;
    uint16_t x;
    uint16_t y;
    uint16_t tarId; 
};

struct coor {
    int x;
    int y;

    bool operator==(coor& c) {
        return x == c.x && y == c.y;
    }
    bool operator!=(coor& c) {
        return !(x == c.x && y == c.y);
    }
};

struct pathInfo {
    gColor          color;
    gPathType       type;
    vector<coor>    path;
};

class GridPlanner {
public:
    GridPlanner(void);
    GridPlanner(uint16_t WW, uint16_t WH, uint16_t GCNs, uint16_t GRNs);
    GridPlanner(char* cmdStr);
    ~GridPlanner();

    void run(void);
    void loadGridMap(double OR);
    void loadGridMap(string mapPath);
    void createGridMap(uint16_t WW, uint16_t WH, uint16_t GCNs, uint16_t GRNs, string mapPath);
    void setFramerate(uint8_t rate);
    void setShowGridLine(bool flag);
    void addGridAgent(uint16_t id, uint16_t x, uint16_t y);
    void addGridAgent(uint16_t id, coor pos);
    void addGridTarget(uint16_t id, uint16_t x, uint16_t y);
    void addGridTarget(uint16_t id, coor pos);
    bool checkCoorValid(int x, int y);
    void setAgentPos(uint16_t id, coor pos);
    void addShowPath(vector<coor> path, gPathType type, gColor lcolor);
    bool getGridShouldQuit(void);
    vector<vector<gState>> getGridMap(void);
    
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    vector<vector<gState>> gridMap;
    unordered_map<int, GridAgent> gridAgents;
    unordered_map<int, GridAgent> gridTargets;
    vector<pathInfo> gridPath;
    uint16_t windowWidth;
    uint16_t windowHeight;
    uint16_t gridColNums;
    uint16_t gridRowNums;
    uint16_t gridWidth;
    uint16_t gridHeight;
    double   obstacleRate;
    bool     gridShouldQuit = false;
    bool     showGridLine = true;
    uint8_t  frameRate = 30;

    bool isMouseDrag = false;
    SDL_Point mouseDownPoint;
    SDL_Point mouseCurPoint;

    bool agentCanBeOerlap = true;

    std::mutex gridMap_Mutex;
    std::mutex gridPath_Mutex;

    void gridPlannerRun(void);
    void gridPlanSDLInit(void);
    void drawGrid(void);
    void gridClear(void);
    void drawGridLine(void);
    void drawGridPath(void);
    void drawGridRect(uint16_t x, uint16_t y, enum gColor gc);
    void getRectPonit(SDL_Point p1, SDL_Point p2, SDL_Point* sp, SDL_Point* ep);
    SDL_Point getGridXY(int32_t mouseX, int32_t mouseY);
    void saveMap(const string& filename, const vector<vector<gState>>& map);
    bool checkFilePath(const string& filename);
    
};

#endif

