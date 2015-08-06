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

    // calculate position from current parameters
    glm::vec2 locate(double time_1, double time_2, double time_3, double time_4);
    // return error distribution fo the last locate call
    std::vector<std::vector<float>> const& getErrorDistribution() const;
    
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
    glm::tvec2<std::size_t> num_steps_;
    // error per cell
    std::vector<std::vector<float>> error_mapping_;
};

#endif