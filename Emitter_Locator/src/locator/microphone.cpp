
#include "microphone.hpp"


void Microphone::set_toa(double in_time)
{
	toa = in_time;
};


void Microphone::print() const
{
	std::cout << "Microphone at (" << position.x << ", " << position.y << ")\n";
};

