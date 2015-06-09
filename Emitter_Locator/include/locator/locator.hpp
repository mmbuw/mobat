#ifndef LOCATOR_H
#define LOCATOR_H

#include <iostream>
#include <utility>
#include <functional>
#include <vector>
#include <math.h>
#include <sstream>
#include <glm/glm.hpp>


#include "microphone.hpp"

class Locator{
  public:

  	Locator(double c, Microphone const& m1, Microphone const& m2, Microphone const& m3, Microphone const& m4, double minx, double maxx, double miny, double maxy)	:
  		c_{c},
  		mics_{m1, m2, m3, m4},
  		min_x_{minx},	//could also be computed from mics
  		max_x_{maxx},
  		min_y_{miny},
  		max_y_{maxy}
  	{}

    void set_c(double c);
  	void update_times(double a, double b, double c, double d);
  	double norm(glm::vec2 const& a, glm::vec2 const& b) const;
  	double dif(glm::vec2 const& p, Microphone const& mic1, Microphone const& mic2) const;
  	glm::vec2 locate() const;
  	


  private:
  	double c_;
  	Microphone mics_[4];
  	double min_x_;
  	double max_x_;
  	double min_y_;
  	double max_y_;



};




#endif