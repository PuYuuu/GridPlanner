#include "GridPlanner.h"
#include <malloc.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <random>
#include <GL/gl.h>
#include <boost/program_options.hpp>  

namespace bpo = boost::program_options;  

vector<vector<uint8_t>> colorLib = {{0xFF,0xFF,0xFF},{0x00,0x00,0x00},{0xFF,0x00,0x00},
    {0xFF,0x99,0x00},{0xFF,0xFF,0x00},{0x00,0xFF,0x00},{0x93,0xC4,0x7D},{0x00,0x00,0xFF},
    {0x67,0x4E,0xA7},{0xCC,0xCC,0xCC},{0x7F,0x60,0x00}};

void convertCmdStr(char* str, char* args[], uint8_t& argc)
{
    uint8_t index = 0;
    bool flag = false;
    argc = 0;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] != ' ') {
            flag = true;
            args[argc][index++] = str[i];
        } else if (flag && str[i] == ' ') {
            flag = false;
            argc++;
            index = 0;
        }
    }
    if (flag) {
        argc++;
    }
}

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

GridPlanner::GridPlanner(char* cmdStr)
{
    uint8_t argc = 0;
    char** args = (char**)malloc(sizeof(char*) * 20);
    bpo::options_description opt("GridPlanning Options");  

    for (int i = 0; i < 20; ++i) {
        args[i] = (char*)malloc(sizeof(char) * 30);
    }
    convertCmdStr(cmdStr, args, argc);

    opt.add_options()
        ("windowWidth,WW",   bpo::value<uint16_t>(&windowWidth),  "The grid window width")
        ("windowHeight,WH",  bpo::value<uint16_t>(&windowHeight), "The grid window height")
        ("gridColNums,GCNs", bpo::value<uint16_t>(&gridColNums),  "The grid column nums")
        ("gridRowNums,GRNs", bpo::value<uint16_t>(&gridRowNums),  "The grid row nums")
        ("obstacleRate,OR",  bpo::value<double>(&obstacleRate)->default_value(0.2), 
                            "The rate of obstacle in grid");
    bpo::variables_map vm;
    try{  
        bpo::store(parse_command_line(argc, args, opt), vm);  
    }  
    catch(...){  
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "GridPlanning parameter parse ERROR\n");
        exit(-1);  
    } 
    bpo::notify(vm); 

    if (vm.count("help")) {
        std::cout << opt << std::endl;
    }

    for (int i = 0; i < 20; ++i) {
        free(args[i]);
    }
    free(args);
}

GridPlanner::~GridPlanner()
{
    
}

void GridPlanner::gridPlanningRun(void)
{   
    gridPlanSDLInit();

    while (!gridShouldQuit) {
        uint32_t start = SDL_GetTicks();
        SDL_Event event;    // 事件
        
        gridClear();
        drawGridLine();
        drawGrid();
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
    std::thread GT(&GridPlanner::gridPlanningRun, this);
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

void GridPlanner::addGridAgent(uint16_t id, coor agent)
{
    std::unique_lock<std::mutex> addGridAgent_Lock(gridMap_Mutex);

    if (checkCoorValid(agent.x, agent.y)) {
        gridAgents[id].x = agent.x;
        gridAgents[id].y = agent.y;
        gridMap[agent.y][agent.x] = OCCUPY_A;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is invalid!");
    }
}

void GridPlanner::addGridTarget(uint16_t id, uint16_t x, uint16_t y)
{
    std::unique_lock<std::mutex> addGridTarget_Lock(gridMap_Mutex);

    if (checkCoorValid(x, y)) {
        gridTargets[id].x = x;
        gridTargets[id].y = y;
        gridMap[y][x] = TARGET;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is invalid!");
    }
}

void GridPlanner::addGridTarget(uint16_t id, coor agent)
{
    std::unique_lock<std::mutex> addGridTarget_Lock(gridMap_Mutex);

    if (checkCoorValid(agent.x, agent.y)) {
        gridTargets[id].x = agent.x;
        gridTargets[id].y = agent.y;
        gridMap[agent.y][agent.x] = TARGET;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "The input coordinate is invalid!");
    }
}


void GridPlanner::gridPlanSDLInit(void)
{
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, 
            "SDL could not initialize! SDL_Error:%s!\n", SDL_GetError());
        exit(-1);
    } 

    window = SDL_CreateWindow( "GridPlanning", SDL_WINDOWPOS_UNDEFINED, 
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
    SDL_Point coor;
    coor.x = -1, coor.y = -1;
    if (mouseX > 0 && mouseY > 0 && mouseX <= windowWidth && mouseY <= windowHeight) {
        coor.x = mouseX / gridWidth;
        coor.y = mouseY / gridHeight;
    }
    return coor;
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
    std::regex r("^[./0-9a-zA-Z-_]+$");
#elif _WIN32
    std::regex r("^[./\\:0-9a-zA-Z-_]+$");
#endif
    
    return regex_match(filename, r);
}

bool GridPlanner::checkCoorValid(int x, int y)
{
    if (x >= gridColNums || x < 0 || y >= gridRowNums || y < 0) {
        return false;
    } else if (gridMap[y][x] == OCCUPY_O || 
        (!agentCanBeOerlap && gridMap[y][x] == OCCUPY_A)){
        return false;
    }
    return true;
}