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

  	Locator()	:
  		m_c{0},
  		m_mics{{0,0}, {0,0}, {0,0}, {0,0}},
  		m_min_x{0},
  		m_max_x{0},
  		m_min_y{0},
  		m_max_y{0}

  	{}

  	Locator(double c, microphone const& m1, microphone const& m2, microphone const& m3, microphone const& m4, double minx, double maxx, double miny, double maxy)	:
  		m_c{c},
  		m_mics{m1, m2, m3, m4},
  		m_min_x{minx},	//could also be computed from mics
  		m_max_x{maxx},
  		m_min_y{miny},
  		m_max_y{maxy}
  	{}

  	void update_times(double a, double b, double c, double d);
  	double betrag(glm::vec2 const& a, glm::vec2 const& b) const;
  	double dif(glm::vec2 const& p, microphone const& mic1, microphone const& mic2) const;
  	glm::vec2 locate() const;
  	


  private:
  	double m_c;
  	microphone m_mics[4];
  	double m_min_x;
  	double m_max_x;
  	double m_min_y;
  	double m_max_y;



};




#endif