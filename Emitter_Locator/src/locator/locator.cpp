#include "locator.hpp"


void Locator::
set_c(double in_c) {
	c_ = in_c;
}

void Locator::
update_times(double a, double b, double c, double d) {
	mics_[0].set_toa(a);
  	mics_[1].set_toa(b);
  	mics_[2].set_toa(c);
  	mics_[3].set_toa(d);
};


double Locator::
norm(glm::vec2 const& a, glm::vec2 const& b) const {
	return(sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
};

double Locator::
dif(glm::vec2 const& p, 
					Microphone const& mic1, 
					Microphone const& mic2) const {
	double dtoa = (mic1.toa_ - mic2.toa_);
	glm::vec2 p_mic_1{mic1.x_, mic1.y_};
	glm::vec2 p_mic_2{mic2.x_, mic2.y_};
	//std::cout<<"betr "<<betrag(p , p_mic_1)<<"    "<<betrag(p , p_mic_2)<<"    "<<c*dtoa<<"\n";
	return this->norm(p , p_mic_1) - this->norm(p , p_mic_2) - c_ * dtoa;
};

glm::vec2 Locator::
locate() const {
	int min_x = -1;
	int min_y = -1;
	double min_dif = 1000000;
	double temp_dif = 0;
	glm::vec2 test;

	double min_x_mic_pos = std::min(std::min(std::min(mics_[0].x_ , mics_[1].x_), mics_[2].x_), mics_[3].x_);
	double max_x_mic_pos = std::max(std::max(std::max(mics_[0].x_ , mics_[1].x_), mics_[2].x_), mics_[3].x_);

	double min_y_mic_pos = std::min(std::min(std::min(mics_[0].y_ , mics_[1].y_), mics_[2].y_), mics_[3].y_);
	double max_y_mic_pos = std::max(std::max(std::max(mics_[0].y_ , mics_[1].y_), mics_[2].y_), mics_[3].y_);

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

