#ifndef MICROPHONE
#define MICROPHONE

//set default glm types double and request highest precision 
#define GLM_PRECISION_HIGHP_DOUBLE
#include <glm/glm.hpp>
#include <iostream>

struct Microphone
{
	Microphone() :
	 position{0, 0}
	{}

	Microphone(double x, double y) :
		position{x, y}
	{}

	void print() const;


	glm::vec2 position;
	double toa;		//time of arrival
};


#endif