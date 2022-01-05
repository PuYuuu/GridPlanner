#include "GridPlanner.h"
#include <malloc.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <random>

#ifdef _WIN32
    #include <Windows.h>
#endif
#include <GL/gl.h>  

vector<vector<uint8_t>> colorLib = {{0xFF,0xFF,0xFF},{0x00,0x00,0x00},{0xFF,0x00,0x00},
    {0xFF,0x99,0x00},{0xFF,0xFF,0x00},{0x00,0xFF,0x00},{0x93,0xC4,0x7D},{0x00,0x00,0xFF},
    {0x67,0x4E,0xA7},{0xCC,0xCC,0xCC},{0x7F,0x60,0x00}};

GridPlanner::GridPlanner(void)
{
    
}

GridPlanner::GridPlanner(uint16_t WW, uint16_t WH, uint16_t GCNs, uint16_t GRNs) :
    windowWidth(WW), windowHeight(WH), gridColNums(GCNs), gridRowNums(GRNs)
{
    gridWidth = (uint16_t) windowWidth / gridColNums;
    gridHeight = (uint16_t) windowHeight / gridRowNums;
    gridMap.resize(gridRowNums);
    for (auto& r : gridMap) {
        r.resize(gridColNums);
    }
}

GridPlanner::~GridPlanner()
{
    
}

void GridPlanner::gridPlannerRun(void)
{   
    gridPlanSDLInit();

    while (!gridShouldQuit) {
        uint32_t start = SDL_GetTicks();
        SDL_Event event;    // 事件
        
        gridClear();
        drawGridLine();
        drawGrid();
        drawGridPath();
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gridShouldQuit = true;
                break;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    gridShouldQuit = true;
                    break;
                }
            }
        }
        uint32_t end = SDL_GetTicks();
        uint32_t time = end - start;
        uint32_t rate = 1000 / frameRate;
        SDL_Delay(rate - time);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GridPlanner::run(void)
{
    std::thread GT(&GridPlanner::gridPlannerRun, this);
    GT.detach(); 
}

void GridPlanner::loadGridMap(double OR = 0.2)
{
    if (OR >= 0 && OR < 1) {
        obstacleRate = OR;
        uint32_t totalGridNum = gridColNums * gridRowNums;
        uint32_t obsNum = (uint32_t)(static_cast<double>(totalGridNum) * OR);
        std::random_device sd;                  
        std::minstd_rand linearRan(sd());       
        std::uniform_int_distribution<uint32_t> dis(0, totalGridNum - 1);   
        for(uint32_t i = 0;i < obsNum; ++i){
            uint32_t randomNum = dis(linearRan);
            uint16_t randRow = randomNum / gridRowNums;
            uint16_t randCol = randomNum % gridRowNums;
            gridMap[randRow][randCol] = OCCUPY_O;
        }
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "ObstacleRate setting OUT OF RANGE\n");
    }
}

void GridPlanner::loadGridMap(string mapPath)
{
    if (!checkFilePath(mapPath)) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input file path is invalid !");
        return ;
    }

    std::ifstream fin(mapPath.c_str()); 
    if (!fin.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Open the map file failed !");
    }

    fin >> windowWidth >> windowHeight >> gridRowNums >> gridColNums;
    gridWidth = (uint16_t) windowWidth / gridColNums;
    gridHeight = (uint16_t) windowHeight / gridRowNums;
    gridMap.resize(gridRowNums);
    for (auto& r : gridMap) {
        r.resize(gridColNums);
    }
    int gStateTmp;
    for (int i = 0; i < gridRowNums; ++i) {
        for (int j = 0; j < gridColNums; ++j) {
            fin >> gStateTmp;
            gridMap[i][j] = (gState)gStateTmp;
        }
    }
    fin.close();
}

void GridPlanner::createGridMap(uint16_t WW, uint16_t WH, uint16_t GCNs, uint16_t GRNs, string mapPath)
{
    if (!checkFilePath(mapPath)) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input file path is invalid !");
        return ;
    }

    windowWidth = WW, windowHeight = WH, gridColNums = GCNs, gridRowNums = GRNs;
    gridWidth = (uint16_t) windowWidth / gridColNums;
    gridHeight = (uint16_t) windowHeight / gridRowNums;
    gridMap.resize(gridRowNums);
    for (auto& r : gridMap) {
        r.resize(gridColNums);
    }

    SDL_Log("push 's' to save the map and exit");

    gridPlanSDLInit();
    while (!gridShouldQuit) {
        uint32_t start = SDL_GetTicks();
        SDL_Event event;            
        SDL_Point sp, ep, spG, epG;
        SDL_Rect rect;

        gridClear();
        drawGridLine();
        drawGrid();
        if (isMouseDrag) {
            SDL_SetRenderDrawColor(renderer, colorLib[RED][0], colorLib[RED][1], colorLib[RED][2], 0xFF);
            getRectPonit(mouseDownPoint, mouseCurPoint, &sp, &ep);
            rect = {sp.x, sp.y, ep.x - sp.x, ep.y - sp.y};
            SDL_RenderDrawRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:
                gridShouldQuit = true;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    gridShouldQuit = true;
                } else if (event.key.keysym.sym == SDLK_s) {
                    saveMap(mapPath, gridMap);
                    gridShouldQuit = true;
                }
                break;
            case SDL_MOUSEMOTION:
                if (isMouseDrag) {
                    mouseCurPoint.x = event.motion.x;
                    mouseCurPoint.y = event.motion.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                isMouseDrag = false;
                mouseCurPoint.x = event.motion.x;
                mouseCurPoint.y = event.motion.y;
                getRectPonit(mouseDownPoint, mouseCurPoint, &sp, &ep);
                spG = getGridXY(sp.x, sp.y);
                epG = getGridXY(ep.x, ep.y);
                for (int i = spG.y; i <= epG.y; ++i) {
                    for (int j = spG.x; j <= epG.x; ++j) {
                        if (gridMap[i][j] == IDLE) {
                            gridMap[i][j] = OCCUPY_O;
                        } else if (gridMap[i][j] == OCCUPY_O) {
                            gridMap[i][j] = IDLE;
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                isMouseDrag = true;
                mouseDownPoint.x = event.motion.x;
                mouseDownPoint.y = event.motion.y;
                mouseCurPoint.x = event.motion.x;
                mouseCurPoint.y = event.motion.y;
                break;
            default:
                break;
            }
        }

        uint32_t end = SDL_GetTicks();
        uint32_t time = end - start;
        uint32_t rate = 1000 / frameRate;
        SDL_Delay(rate - time);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GridPlanner::setFramerate(uint8_t rate)
{
    frameRate = rate;
}

void GridPlanner::setShowGridLine(bool flag)
{
    showGridLine = flag;
}

void GridPlanner::addGridAgent(uint16_t id, uint16_t x, uint16_t y)
{
    std::unique_lock<std::mutex> addGridAgent_Lock(gridMap_Mutex);

    if (checkCoorValid(x, y)) {
        gridAgents[id].x = x;
        gridAgents[id].y = y;
        gridMap[y][x] = OCCUPY_A;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is invalid!");
    }
}

void GridPlanner::addGridAgent(uint16_t id, coor pos)
{
    std::unique_lock<std::mutex> addGridAgent_Lock(gridMap_Mutex);

    if (checkCoorValid(pos.x, pos.y)) {
        gridAgents[id].x = pos.x;
        gridAgents[id].y = pos.y;
        gridMap[pos.y][pos.x] = OCCUPY_A;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is invalid!");
    }
}

void GridPlanner::addGridAgents(int count, ...)
{
    va_list args;
	va_start(args, count);
    for(int i = 0; i < count; ++i) {
        addGridAgent(gridAgents.size(), va_arg(args, coor));
    }
	va_end(args);
}

void GridPlanner::addGridTarget(uint16_t id, uint16_t x, uint16_t y)
{
    std::unique_lock<std::mutex> addGridTarget_Lock(gridMap_Mutex);

    if (checkCoorValid(x, y)) {
        gridTargets[id].x = x;
        gridTargets[id].y = y;
        gridMap[y][x] = TARGET;
    }
}

void GridPlanner::addGridTarget(uint16_t id, coor pos)
{
    std::unique_lock<std::mutex> addGridTarget_Lock(gridMap_Mutex);

    if (checkCoorValid(pos.x, pos.y)) {
        gridTargets[id].x = pos.x;
        gridTargets[id].y = pos.y;
        gridMap[pos.y][pos.x] = TARGET;
    } 
}

void GridPlanner::addGridTargets(int count, ...)
{
    va_list args;
	va_start(args, count);
    for(int i = 0; i < count; ++i) {
        addGridTarget(gridTargets.size(), va_arg(args, coor));
    }
	va_end(args);
}

bool GridPlanner::checkCoorValid(int x, int y) const
{
    if (x >= gridColNums || x < 0 || y >= gridRowNums || y < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is out of range!");
        return false;
    } else if (gridMap[y][x] == OCCUPY_O || 
        (!agentCanBeOerlap && gridMap[y][x] == OCCUPY_A)){
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "There is a obstacle!");
        return false;
    }
    return true;
}

void GridPlanner::setAgentPos(uint16_t id, coor pos)
{
    std::unique_lock<std::mutex> addGridAgent_Lock(gridMap_Mutex);

    if (!gridAgents.count(id)) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The agent ID is invalid!");
    }
    if (checkCoorValid(pos.x, pos.y)) {
        gridMap[gridAgents[id].y][gridAgents[id].x] = IDLE;
        gridAgents[id].x = pos.x;
        gridAgents[id].y = pos.y;
        gridMap[pos.y][pos.x] = OCCUPY_A;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is invalid!");
    }    
}

void GridPlanner::addShowPath(vector<coor> path, gPathType type = pLINE, gColor pcolor = GREEN)
{
    pathInfo pTmp;
    std::unique_lock<std::mutex> addShowPath_Lock(gridPath_Mutex);

    pTmp.color = pcolor;
    pTmp.type = type;
    pTmp.path = path;
    gridPath.emplace_back(pTmp);
}

bool GridPlanner::getGridShouldQuit(void) const
{
    return gridShouldQuit;
}

vector<vector<gState>> GridPlanner::getGridMap(void) const
{
    return gridMap;
}

void GridPlanner::gridPlanSDLInit(void)
{
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, 
            "SDL could not initialize! SDL_Error:%s!\n", SDL_GetError());
        exit(-1);
    } 

    window = SDL_CreateWindow( "GridPlanner", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN );
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Create window failed!\n");
        exit(-1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Create renderer failed!\n");
        exit(-1);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void GridPlanner::drawGrid(void)
{
    std::unique_lock<std::mutex> drawGrid_Lock(gridMap_Mutex);

    for (int i = 0; i < gridRowNums; ++i) {
        for (int j = 0; j < gridColNums; ++j) {
            switch (gridMap[i][j])
            {
            case IDLE:
                
                break;
            case OCCUPY_O:
                drawGridRect(j, i, GRAY);
                break;
            case OCCUPY_A:
                drawGridRect(j, i, RED);
                break;
            case TARGET:
                drawGridRect(j, i, BLUE);
                break;
            default:
                break;
            }
        }
    }
}

void GridPlanner::gridClear(void)
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}

void GridPlanner::drawGridLine(void)
{
    if (showGridLine) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        glLineWidth(2);
        for (int i = 0; i <= gridRowNums; ++i) {
            SDL_RenderDrawLine(renderer, 0, i * gridHeight, windowWidth, i * gridHeight);
        }
        for (int i = 0; i <= gridColNums; ++i) {
            SDL_RenderDrawLine(renderer, i * gridWidth, 0, i * gridWidth, windowHeight);
        }
    }
}

void GridPlanner::drawGridPath(void)
{
    std::unique_lock<std::mutex> drawGridPath_Lock(gridPath_Mutex);
    
    if (gridPath.empty()) {
        return ;
    }


    for (auto& p : gridPath) {
        if (p.type == pLINE) {
            SDL_SetRenderDrawColor(renderer, colorLib[p.color][0], 
                colorLib[p.color][1], colorLib[p.color][2], 0xFF);
            glLineWidth(3);
            for (int i = 0; i < (p.path.size() - 1); ++i) {
                SDL_RenderDrawLine(renderer, (p.path[i].x + 0.5) * gridWidth, (p.path[i].y + 0.5) * gridHeight, 
                    (p.path[i + 1].x + 0.5) * gridWidth, (p.path[i + 1].y + 0.5) * gridHeight);
            }
        } else {
            // for (auto& g : p.path) {
            //     drawGridRect(g.x, g.y, p.color);
            // }
            for (int i = 0; i < (p.path.size() - 1); ++i) {
                drawGridRect(p.path[i].x, p.path[i].y, p.color);
            }
        }
    }
}

void GridPlanner::drawGridRect(uint16_t x, uint16_t y, enum gColor gc)
{
    if (x < 0 || x >= gridColNums || y < 0 || y >= gridRowNums) {
        SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Input 'x' or 'y' out of range!");
        return ;
    }
    SDL_SetRenderDrawColor(renderer, colorLib[gc][0], colorLib[gc][1], colorLib[gc][2], 0xFF);
    uint16_t rectX, rectY = y * gridHeight;
    SDL_Rect rect;

    if (showGridLine) {
        rectX = x * gridWidth + 1;
        rectY = y * gridHeight + 2;
        rect = {rectX, rectY, gridWidth - 2, gridHeight - 2};
    } else {
        rectX = x * gridWidth;
        rectY = y * gridHeight;
        rect = {rectX, rectY, gridWidth, gridHeight};
    }

    SDL_RenderFillRect(renderer, &rect);
}

void GridPlanner::getRectPonit(SDL_Point p1, SDL_Point p2, SDL_Point* sp, SDL_Point* ep)
{
    sp->x = std::min(p1.x, p2.x);
    sp->y = std::min(p1.y, p2.y);
    ep->x = std::max(p1.x, p2.x);
    ep->y = std::max(p1.y, p2.y);
}

SDL_Point GridPlanner::getGridXY(int32_t mouseX, int32_t mouseY)
{
    SDL_Point p;
    p.x = -1, p.y = -1;
    if (mouseX > 0 && mouseY > 0 && mouseX <= windowWidth && mouseY <= windowHeight) {
        p.x = mouseX / gridWidth;
        p.y = mouseY / gridHeight;
    }
    return p;
}

void GridPlanner::saveMap(const string& filename,const vector<vector<gState>>& map)
{
    int m = map.size(), n = map[0].size();
    std::ofstream fout(filename.c_str());
    fout << windowWidth << " " << windowHeight << " " << 
        gridRowNums << " " << gridColNums << std::endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            fout << map[i][j] << " ";
        }
        fout << std::endl;
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
        "Map saver", "Map saved successfully !", NULL);
    fout.close();
}

bool GridPlanner::checkFilePath(const string& filename)
{
#ifdef __linux__
    std::regex r("^[./0-9a-zA-Z-_*]+$");
#elif _WIN32
    std::regex r("^[./\\:0-9a-zA-Z-_*]+$");
#endif
    
    return regex_match(filename, r);
}

