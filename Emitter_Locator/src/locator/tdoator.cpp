#include "tdoator.hpp"

#include <iostream>
#include <math.h>

 TDOAtor::
 TDOAtor(double c,
   Microphone const& m1, Microphone const& m2,
   Microphone const& m3, Microphone const& m4):
    c_{c},
    mics_{m1, m2, m3, m4}
{
    min_.x = std::min(std::min(std::min(mics_[0].position.x , mics_[1].position.x), mics_[2].position.x), mics_[3].position.x);
    max_.x = std::max(std::max(std::max(mics_[0].position.x , mics_[1].position.x), mics_[2].position.x), mics_[3].position.x);

    min_.y = std::min(std::min(std::min(mics_[0].position.y , mics_[1].position.y), mics_[2].position.y), mics_[3].position.y);
    max_.y = std::max(std::max(std::max(mics_[0].position.y , mics_[1].position.y), mics_[2].position.y), mics_[3].position.y);

/*
    for(unsigned mic_1_idx = 0; mic_1_idx < 4; ++mic_1_idx ) {
        for(unsigned mic_2_idx = 0; mic_2_idx < 4; ++mic_2_idx ) {
            //calculates the time of arrival that should be measured from one microphone to the other
            distance_mic_low_to_high_[mic_1_idx][mic_2_idx] = glm::length(mics_[mic_1_idx].position - mics_[mic_2_idx].position) / 44100;
        }
    }
*/
}

void TDOAtor::
set_c(double in_c) {
    c_ = in_c;
}

void TDOAtor::
update_times(double a, double b, double c, double d) {
    mics_[0].toa = a;
    mics_[1].toa = b;
    mics_[2].toa = c;
    mics_[3].toa = d;
};

double TDOAtor::
dif(glm::vec2 const& p, 
                    Microphone const& mic1, 
                    Microphone const& mic2) const {
    double dtoa = (mic1.toa - mic2.toa) ;// 1000.0;

    return dtoa - (glm::length(p - mic1.position) - glm::length(p - mic2.position) ) / c_ ;
};

glm::vec2 TDOAtor::
locate() const {


    double min_x = min_.x;
    double min_y = min_.y;
  //  double min_dif = 10000000;

    glm::vec2 test;

    double x_step_length = (max_.x - min_.x) / 100.0;
    double y_step_length = (max_.y - min_.y) / 200.0;

    std::map<unsigned, std::map<unsigned, double> > xy_counter_to_error_mapping;
    unsigned x_counter = 0;
    unsigned y_counter = 0;

    for(double x = min_.x; x <= max_.x; x += x_step_length, ++x_counter/*x=x+10*/ ) {
        test.x = x;
        y_counter = 0;
        for(double y = min_.y; y <= max_.y; y += y_step_length, ++y_counter/*y=y+10*/) {
            test.y = y;


            double temp_dif = 0.0;
            
            for(int i = 0; i < 3; ++i) {
                for(int j = i+1; j < 4; ++j) {


                    temp_dif += fabs(dif(test, mics_[i], mics_[j]));

                }
            }
            
            xy_counter_to_error_mapping[x_counter][y_counter] = temp_dif;
            /*
              if(temp_dif < min_dif) {
                  min_dif = temp_dif;
                  min_x = x;
                  min_y = y;
              }
            */
            temp_dif = 0;
        }
    }

    unsigned largest_x = x_counter - 1;
    unsigned largest_y = y_counter - 1;

    double least_error = std::numeric_limits<double>::max();
    unsigned least_error_x_count = 0;
    unsigned least_error_y_count = 0;
    x_counter = 0;
    y_counter = 0;


    /*
         +  : sample points
        --- : horizontal axis between sample points
         |  : vertical axis between sample points
         o  : possible new position

        +---+---+---+---+---+
        | o | o | o | o | o |
        +---+---+---+---+---+
        | o | o | o | o | o |
        +---+---+---+---+---+
        | o | o | o | o | o |
        +---+---+---+---+---+

        compute the error between 4 corners in order to determine a 
        point in-between in order to avoid jumping within the cell
    */

    for(unsigned x_it = 0; x_it < largest_x; ++x_it) {

        for(unsigned y_it = 0; y_it < largest_y; ++y_it) {

            double tile_error =  xy_counter_to_error_mapping[x_it][y_it] 
                               + xy_counter_to_error_mapping[x_it][y_it+1]
                               + xy_counter_to_error_mapping[x_it+1][y_it]
                               + xy_counter_to_error_mapping[x_it+1][y_it+1];//*(y_iterator+1) + ( (*(xy_iterator+1)) )

            if (tile_error < least_error) {
                least_error = tile_error;
                least_error_x_count = x_it;
                least_error_y_count = y_it;
            }

            //++y_counter;
        }

        //++x_counter;
    }

    //std::cout << "Returning: "  << min_x + least_error_x_count * x_step_length << ", " << min_y + least_error_y_count * y_step_length << "\n";
    return {min_x + least_error_x_count * x_step_length, 
            min_y + least_error_y_count * y_step_length}; 

    //return {min_x, min_y};
};

