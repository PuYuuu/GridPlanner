#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <SDL2/SDL.h>
// #include <unistd.h>

#ifdef _WIN32
    #include <Windows.h>
#endif
#include <GL/gl.h>
using namespace std;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr uint8_t FRAMERATE = 30;

SDL_Window* window = nullptr;
SDL_Renderer *renderer = nullptr;

void draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    glLineWidth(10);
    SDL_RenderDrawLine(renderer, 0,0, 200, 200);

    SDL_RenderPresent(renderer);
}

void test(void)
{
    const SDL_MessageBoxButtonData buttons[] = {
        { /* .flags, .buttonid, .text */        0, 0, "no" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 255,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            {   0, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 255, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            {   0,   0, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 255,   0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "example message box", /* .title */
        "select a button", /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        exit(-1);
    } 

    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (!window) {
        cerr << "Create window failed!" << endl;
        exit(-1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Cannot create renderer !" << endl;
        exit(-1);
    }


    while (1) {
        uint32_t start = SDL_GetTicks();
        SDL_Event event;    // 事件
        
        draw(renderer);
        // if (SDL_WaitEvent(&event)) {        // 阻塞读取
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(-1);
                break;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    exit(-1);
                    break;
                } else if (event.key.keysym.sym == SDLK_a) {
                    SDL_Log("You putdown a");
                    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
                        SDL_Log("error displaying message box");
                        return ;
                    }
                    if (buttonid == -1) {
                        SDL_Log("no selection");
                    } else {
                        SDL_Log("selection was %s", buttons[buttonid].text);
                    }
                } else if (event.key.keysym.sym == SDLK_s) {
                    SDL_Log("You putdown b");
                    SDL_Rect rect = {100, 100, 200, 50};
                    SDL_SetTextInputRect(&rect);
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                         "Missing file",
                         "File is missing. Please reinstall the program.",
                         NULL);
                }
            } else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_a) {
                    SDL_Log("You putup a");
                } else if (event.key.keysym.sym == SDLK_b) {
                    
                }
            }
        }
        uint32_t end = SDL_GetTicks();
        uint32_t time = end - start;
        uint32_t rate = 1000 / FRAMERATE;
        if (rate > time ) {
            SDL_Delay(rate - time);
        }
    }
}

int main( int argc, char* args[] )
{
    thread t1(test);
    t1.detach();

    while (true) {
        cout << "main loop\n";
        // sleep(2);
        SDL_Delay(2000);
    }
  
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


