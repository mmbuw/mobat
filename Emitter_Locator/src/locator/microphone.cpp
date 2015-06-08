#ifndef MIC_CPP
#define MIC_CPP	


#include "microphone.hpp"


void microphone::set_toa(double t)
{
	m_toa = t;
};


void microphone::print() const
{
	std::cout << "Microphone at (" << m_x << ", " << m_y << ")\n";
};


#endif