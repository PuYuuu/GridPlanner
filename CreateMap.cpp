#include <string>
#include "GridPlanner.h"
#include "cmdline.h"

int main( int argc, char* args[] )
{
    GridPlanner* gp = new GridPlanner();
    cmdline::parser cmd;

    cmd.add<uint16_t>("wWidth",'w', "The map window width", false, 480);
    cmd.add<uint16_t>("wHeight", 'h', "The map window height", false, 480);
    cmd.add<uint16_t>("colNums", 'c', "The col grid nums", false, 16);
    cmd.add<uint16_t>("rowNums", 'r', "The row gird nums", false, 16);
    cmd.add<std::string>("filename", 'p', "The creating map filename", true);
    cmd.parse_check(argc, args);

    uint16_t wWidth = cmd.get<uint16_t>("wWidth");
    uint16_t wHeight = cmd.get<uint16_t>("wHeight");
    uint16_t colNums = cmd.get<uint16_t>("colNums");
    uint16_t rowNums = cmd.get<uint16_t>("rowNums");
    std::string filename = cmd.get<std::string>("filename"); 
    filename = "../maps/" + filename;

    gp->createGridMap(wWidth,wHeight,colNums, rowNums, filename);

    return 0;
}