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

    Locator(double c,
           Microphone const& m1, Microphone const& m2, Microphone const& m3, Microphone const& m4,
           double minx, double maxx, double miny, double maxy);

    // set the speed of sound
    void set_c(double c);

    // set the signal arrival times
    void update_times(double a, double b, double c, double d);

    // 
    double dif(glm::vec2 const& p, Microphone const& mic1, Microphone const& mic2) const;

    // calculate position from current parameters
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