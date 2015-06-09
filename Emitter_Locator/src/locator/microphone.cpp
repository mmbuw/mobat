
#include "microphone.hpp"


void Microphone::set_toa(double in_time)
{
	toa_ = in_time;
};


void Microphone::print() const
{
	std::cout << "Microphone at (" << x_ << ", " << y_ << ")\n";
};

