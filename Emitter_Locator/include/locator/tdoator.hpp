#ifndef TDOATOR_HPP
#define TDOATOR_HPP

//set default glm types double and request highest precision 
#define GLM_PRECISION_HIGHP_DOUBLE
#include <glm/glm.hpp>

#include <vector>
#include <map>

struct ErrorOf2DPos {
    double error_;
    glm::vec2 pos_;
};

class TDOAtor{
  public:

    TDOAtor(double c,
           glm::vec2 const& m1, glm::vec2 const& m2, glm::vec2 const& m3, glm::vec2 const& m4);

    // set the speed of sound
    void setSoundSpeed(double c);

    // set the signal arrival times
    void update_times(double a, double b, double c, double d);


    // calculate position from current parameters
    glm::vec2 locate() const;
    
  private:
    // calculates the difference of ?
    double dif(glm::vec2 const& p, unsigned m1, unsigned m2) const;

    //m/s
    double c_;

    //with toa in s
    std::vector<glm::vec2> mics_;
    std::vector<float> toas_;
    glm::vec2 min_;
    glm::vec2 max_;

    std::map<unsigned int, std::map<unsigned int, float> > distance_mic_low_to_high_;
};




#endif