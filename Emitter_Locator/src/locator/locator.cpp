#include "locator.hpp"

 Locator::
 Locator(double c,
   Microphone const& m1, Microphone const& m2,
   Microphone const& m3, Microphone const& m4,
   double minx, double maxx, double miny, double maxy):
    c_{c},
    mics_{m1, m2, m3, m4},
    min_x_{minx},   //could also be computed from mics
    max_x_{maxx},
    min_y_{miny},
    max_y_{maxy}
{}

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
    return glm::length(p - mic1.position) - glm::length(p - mic1.position) - c_ * dtoa;
};

glm::vec2 Locator::
locate() const {
    int min_x = -1;
    int min_y = -1;
    double min_dif = 1000000;
    double temp_dif = 0;
    glm::vec2 test;

    double min_x_mic_pos = std::min(std::min(std::min(mics_[0].position.x , mics_[1].position.x), mics_[2].position.x), mics_[3].position.x);
    double max_x_mic_pos = std::max(std::max(std::max(mics_[0].position.x , mics_[1].position.x), mics_[2].position.x), mics_[3].position.x);

    double min_y_mic_pos = std::min(std::min(std::min(mics_[0].position.y , mics_[1].position.y), mics_[2].position.y), mics_[3].position.y);
    double max_y_mic_pos = std::max(std::max(std::max(mics_[0].position.y , mics_[1].position.y), mics_[2].position.y), mics_[3].position.y);

    for(double x = min_x_mic_pos; x <= max_x_mic_pos; ++x/*x=x+10*/ ) {
        test.x = x;
        for(double y = min_y_mic_pos; y <= max_y_mic_pos; ++y/*y=y+10*/) {
            test.y = y;
            for(int i = 0; i < 3; ++i) {
                for(int j = i+1; j < 4; ++j) {
                    //std::cout<<fabs(dif(test, mics[i], mics[j]))<<"   ";
                    temp_dif += fabs(this->dif(test, mics_[i], mics_[j]));
                }
            }
            
    
            if(fabs(temp_dif) < min_dif) {
                min_dif = fabs(temp_dif);
                min_x = x;
                min_y = y;
            }
            temp_dif = 0;
        }
    }
    //std::cout<<"("<<min_x<<", "<<min_y<<")   "<<min_dif<<"\n";
    return {min_x, min_y};
};

