#include "score.hpp"

#include <iostream>


namespace TTT {

Score::Score(int maxpoints){
	glm::vec2  min{pixel_projection_offset_.x , pixel_projection_offset_.y };
	glm::vec2  max = min  + glm::vec2{ pixel_projection_size_.x, pixel_projection_size_.y } ;
	color_red_ = sf::Color(255, 0, 0, 128);
	color_blue_ = sf::Color(0, 0, 255, 128);
	maxpoints_ = maxpoints; 

	for(int i = 0; i < maxpoints_; ++i){
		points_.push_back(sf::RectangleShape());
	}

	sf::Vector2f size{pixel_projection_size_.y / (maxpoints*2) , pixel_projection_size_.y / (maxpoints*2)};
	// sf::Vector2f size{max.y - min.y , max.y - min.y};
	if(up_.x == 0){//spiefeld vertikal
		for(int i = 0; i < maxpoints; ++i){
		float distance = (max.x - min.x) / ( 2 * maxpoints_ - 1); 
			// points_.push_back(sf::RectangleShape());
			if(i < maxpoints / 2){
				points_[i].setFillColor(color_blue_);
			}else{
				points_[i].setFillColor(color_red_);
			}

			
			points_[i].setPosition(min.x + distance * i * 2, min.y - size.x/2);	//if table is vertical
			points_[i].setSize(size);
		}
	}else{	//spielfeld horizontal
		float distance = (max.y - min.y) / (points_.size()) - size.x; 
		for(int i = 0; i < maxpoints_; ++i){
		// points_.push_back(sf::RectangleShape());
			if(i < maxpoints / 2){
				points_[i].setFillColor(color_blue_);
			}else{
				points_[i].setFillColor(color_red_);
			}

			points_[i].setPosition((max.x + min.x) / 2 - size.x/2, min.y + size.y/2 +i*distance * 2);  // if table is horizontal
			// points_[i].setPosition((max.x + min.x) / 2 - size.x/2, min.y + min.x);  // if table is horizontal
			points_[i].setSize(size);
		}
	}
}

void Score::
draw(sf::RenderWindow& canvas) const {
	for(auto const& i : points_){

		canvas.draw(i);
	}
}

void Score::
update(int r_goals, int l_goals) {
	int half = maxpoints_ / 2;
	if(r_goals == l_goals)
	{
		points_[half - 1].setFillColor(color_blue_);
		points_[half - 0].setFillColor(color_red_);
	}else{
		int tmp = (r_goals - l_goals);

		if(tmp == - half){
			points_[0].setFillColor(color_red_);
		}else if(tmp == half){
			points_[maxpoints_ - 1].setFillColor(color_blue_);
		}else{
			if(tmp < 0){
				tmp += half;
				points_[tmp-1].setFillColor(color_blue_);
				points_[tmp].setFillColor(color_red_);
			}else{

				tmp += half - 1;
				points_[tmp].setFillColor(color_blue_);
				points_[tmp+1].setFillColor(color_red_);
			}

		}

	}
	

}

void Score::
reset(){
	for(int i = 0; i < maxpoints_; ++i){
		if(i < maxpoints_ / 2){

			points_[i].setFillColor(color_blue_);
		}else{
			points_[i].setFillColor(color_red_);
		}
	}
}

int Score::
getMaxpoints(){
	return maxpoints_;
}



}; //namespace TTT