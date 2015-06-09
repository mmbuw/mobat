#include <iostream>
#include <utility>
#include <functional>
#include <vector>
#include <math.h>
#include <sstream>




#include "microphone.hpp"
#include "locator.hpp"


int main()
{
	std::vector<Microphone> mics = {{0.0, 0.0}, {200, 0.0}, {0.0, 100.0}, {200.0, 100.0}};
	
	#if 1
	//real Values for tap at (40,70) with c=1000 m/s
	mics[0].set_toa(0.0003062);
	mics[1].set_toa(0.0012464);
	mics[2].set_toa(0.0000);
	mics[3].set_toa(0.0011279);

	#else
	//distances for tap at (40,70) with c=1000 m/s
	mics[0].set_toa(80.62);
	mics[1].set_toa(174.64);
	mics[2].set_toa(50);
	mics[3].set_toa(162.79);
	#endif



	Locator loc{100000, mics[0], mics[1], mics[2], mics[3], 0.0, 200.4, 0.0, 150.9};
	glm::vec2 point = loc.locate();
	std::cout << "(" << point.x <<", " << point.y << ")\n";


	return 0;

}