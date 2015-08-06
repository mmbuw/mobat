#include "tdoator.hpp"

#include <iostream>
#include <math.h>
// size of cell in meters
float sample_size = 0.01f;

TDOAtor::
TDOAtor(double c, glm::vec2 const& m1, glm::vec2 const& m2, glm::vec2 const& m3, glm::vec2 const& m4)
 :c_{c}
 ,mics_{m1, m2, m3, m4}
 ,toas_(4, 0)
{
  min_.x = std::min(std::min(std::min(mics_[0].x , mics_[1].x), mics_[2].x), mics_[3].x);
  max_.x = std::max(std::max(std::max(mics_[0].x , mics_[1].x), mics_[2].x), mics_[3].x);

  min_.y = std::min(std::min(std::min(mics_[0].y , mics_[1].y), mics_[2].y), mics_[3].y);
  max_.y = std::max(std::max(std::max(mics_[0].y , mics_[1].y), mics_[2].y), mics_[3].y);
}

void TDOAtor::
setSoundSpeed(double in_c) {
  c_ = in_c;
}

double TDOAtor::
dif(glm::vec2 const& p, unsigned m1, unsigned m2) const {
  double dtoa = (toas_[m1] - toas_[m2]) ;// 1000.0;

  return dtoa - (glm::length(p - mics_[m1]) - glm::length(p - mics_[m2]) ) / c_ ;
};

glm::vec2 TDOAtor::
locate(double time_1, double time_2, double time_3, double time_4) {
  toas_[0] = time_1;
  toas_[1] = time_2;
  toas_[2] = time_3;
  toas_[3] = time_4;

  glm::vec2 located_pos{min_};

  double min_dif = std::numeric_limits<double>::max();

  glm::vec2 curr_pos;

  std::size_t x_steps = (max_.x - min_.x) / sample_size;
  std::size_t y_steps = (max_.y - min_.y) / sample_size;

  std::vector<std::vector<float>> error_mapping(x_steps, std::vector<float>(y_steps, 0.0f));

  for (std::size_t x = 0; x < x_steps; ++x ) {
    curr_pos.x = min_.x + x * sample_size;

    for (std::size_t y = 0; y < y_steps; ++y) {
      curr_pos.y = min_.y + y * sample_size;

      double temp_dif = 0.0;

      for (std::size_t mic_1 = 0; mic_1 < 3; ++mic_1) {
        for (std::size_t mic_2 = mic_1+1; mic_2 < 4; ++mic_2) {
          temp_dif += fabs(dif(curr_pos, mic_1, mic_2));
        }
      }

      error_mapping[x][y] = temp_dif;

      if(temp_dif < min_dif) {
        min_dif = temp_dif;
        located_pos = curr_pos;
      }
    }
  }

/*
    unsigned largest_x = x_counter - 1;
    unsigned largest_y = y_counter - 1;

    double least_error = std::numeric_limits<double>::max();
    unsigned least_error_x_count = 0;
    unsigned least_error_y_count = 0;
    x_counter = 0;
    y_counter = 0;
*/

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
/*
    for(unsigned x_it = 0; x_it < largest_x; ++x_it) {

        for(unsigned y_it = 0; y_it < largest_y; ++y_it) {

            double tile_error =  xy_counter_to_error_mapping[x_it][y_it] 
                               + xy_counter_to_error_mapping[x_it][y_it+1]
                               + xy_counter_to_error_mapping[x_it+1][y_it]
                               + xy_counter_to_error_mapping[x_it+1][y_it+1];

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

*/
  return located_pos;
};

