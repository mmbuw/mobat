#include "locator.hpp"

#include <iostream>
#include <math.h>

 Locator::
 Locator(double c,
   Microphone const& m1, Microphone const& m2,
   Microphone const& m3, Microphone const& m4):
    c_{c},
    mics_{m1, m2, m3, m4}
{
    min_.x = std::min(std::min(std::min(mics_[0].position.x , mics_[1].position.x), mics_[2].position.x), mics_[3].position.x);
    max_.x = std::max(std::max(std::max(mics_[0].position.x , mics_[1].position.x), mics_[2].position.x), mics_[3].position.x);

    min_.y = std::min(std::min(std::min(mics_[0].position.y , mics_[1].position.y), mics_[2].position.y), mics_[3].position.y);
    max_.y = std::max(std::max(std::max(mics_[0].position.y , mics_[1].position.y), mics_[2].position.y), mics_[3].position.y);
}

void Locator::
set_c(double in_c) {
    c_ = in_c;
}

void Locator::
update_times(double a, double b, double c, double d) {
    mics_[0].toa = a;
    mics_[1].toa = b;
    mics_[2].toa = c;
    mics_[3].toa = d;
};

double Locator::
dif(glm::vec2 const& p, 
                    Microphone const& mic1, 
                    Microphone const& mic2) const {
    double dtoa = (mic1.toa - mic2.toa);

    //std::cout<<"betr "<<betrag(p , p_mic_1)<<"    "<<betrag(p , p_mic_2)<<"    "<<c*dtoa<<"\n";
    return glm::length(p - mic1.position) - glm::length(p - mic2.position) - c_ * dtoa;
};

glm::vec2 Locator::
locate() const {
    int min_x = -1;
    int min_y = -1;
    double min_dif = 1000000;
    double temp_dif = 0;
    glm::vec2 test;

    for(double x = min_.x; x <= max_.x; ++x/*x=x+10*/ ) {
        test.x = x;
        for(double y = min_.y; y <= max_.y; ++y/*y=y+10*/) {
            test.y = y;
            for(int i = 0; i < 3; ++i) {
                for(int j = i+1; j < 4; ++j) {
                    //std::cout<<fabs(dif(test, mics[i], mics[j]))<<"   ";
                    temp_dif += fabs(dif(test, mics_[i], mics_[j]));
                }
            }
            
            if(temp_dif < min_dif) {
                min_dif = temp_dif;
                min_x = x;
                min_y = y;
            }
            temp_dif = 0;
        }
    }
    //std::cout<<"("<<min_x<<", "<<min_y<<")   "<<min_dif<<"\n";
    return {min_x, min_y};
};

