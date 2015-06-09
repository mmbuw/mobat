#ifndef MICROPHONE
#define MICROPHONE

#include <iostream>

class Microphone
{
  public:

	Microphone()	:
		x_{0.0},
		y_{0.0}
	{}


	Microphone(double x, double y)	:
		x_{x},
		y_{y}
	{}


	void set_toa(double t);


	void print() const;


  //private not really necessary

	double x_;
	double y_;
	double toa_;		//time of arrival



};


#endif