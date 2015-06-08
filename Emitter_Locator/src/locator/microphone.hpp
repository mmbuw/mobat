#ifndef MICROPHONE
#define MICROPHONE

#include <iostream>

class microphone
{
  public:



	
	microphone()	:
		m_x{0},
		m_y{0}
	{}


	microphone(double x, double y)	:
		m_x{x},
		m_y{y}
	{}


	void set_toa(double t);


	void print() const;


  //private not really necessary

	double m_x;
	double m_y;
	double m_toa;		//time of arrival



};


#endif