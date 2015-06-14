#ifndef MICROPHONE
#define MICROPHONE

//set default glm types to highest precision 
#define GLM_PRECISION_HIGHP_DOUBLE
#include <glm/glm.hpp>
#include <iostream>

struct Microphone
{
  public:

	Microphone() :
	 position{0, 0}
	{}


	Microphone(double x, double y) :
		position{x, y}
	{}


	void set_toa(double t);


	void print() const;


  //private not really necessary

	glm::vec2 position;
	double toa;		//time of arrival



};


#endif