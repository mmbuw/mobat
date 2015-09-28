#include <iostream>
#include <vector>

#include "microphone.hpp"
#include "tdoator.hpp"

int main()
{
    std::vector<Microphone> mics = {{0.0, 0.0}, {200, 0.0}, {0.0, 100.0}, {200.0, 100.0}};
    
    #if 1
    //real Values for tap at (40,70) with c=1000 m/s
    mics[0].toa = 0.0003062;
    mics[1].toa = 0.0012464;
    mics[2].toa = 0.0000;
    mics[3].toa = 0.0011279;

    #else
    //distances for tap at (40,70) with c=1000 m/s
    mics[0].toa = 80.62;
    mics[1].toa = 174.64;
    mics[2].toa = 50)
    mics[3].toa = 162.79;
    #endif



    TDOAtor loc{100000, mics[0], mics[1], mics[2], mics[3]};



    glm::vec2 point = loc.locate();
    std::cout << "(" << point.x <<", " << point.y << ")\n";


    return 0;

}