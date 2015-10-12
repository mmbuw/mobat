#include "score.hpp"

#include <iostream>


namespace MoB {

Score::Score(int max_points){
	glm::vec2  min{pixel_projection_offset_.x , pixel_projection_offset_.y };	//local (0,0)
	glm::vec2  max = min  + glm::vec2{ pixel_projection_size_.x, pixel_projection_size_.y } ;	//maxpoint of loxcal sys
	color_red_ = sf::Color(255, 0, 0, 128);
	color_blue_ = sf::Color(0, 0, 255, 128);
	max_points_ = max_points; 

									//create all necessary ponits and set start colors
	for(int i = 0; i < max_points_; ++i){
		points_.push_back(sf::RectangleShape());
		if(i < max_points / 2){
			points_[i].setFillColor(color_blue_);
		}else{
			points_[i].setFillColor(color_red_);
		}
	}

	sf::Vector2f size{pixel_projection_size_.y / (max_points*2) , pixel_projection_size_.y / (max_points*2)};
	
	if(up_.x == 0){					//field vertical
		float distance = (max.x - min.x) / ( 2 * max_points_ - 1); 
    for(int i = 0; i < max_points; ++i){
									//set position
									//min x + space for curent point and free space, yposition
		  points_[i].setPosition(min.x + distance * i * 2, min.y - size.x/2);	
			points_[i].setSize(size);
		}
	}else{							//field horizontal
		float distance = (max.y - min.y) / (points_.size()) - size.x; 
		for(int i = 0; i < max_points_; ++i){
									//set position
									//mid of x - half pointsize, min y + half y + space for current point and the free space
			points_[i].setPosition((max.x + min.x) / 2 - size.x/2, min.y + size.y/2 +i*distance * 2);
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
	int half = max_points_ / 2;
	if(r_goals == l_goals){			// tie
		points_[half - 1].setFillColor(color_blue_);
		points_[half - 0].setFillColor(color_red_);
	}else{							// untie
		int tmp = (r_goals - l_goals);

		if(tmp == - half){			// left player scores his last point 
			points_[0].setFillColor(color_red_);
		}else if(tmp == half){		// right player scores his last point
			points_[max_points_ - 1].setFillColor(color_blue_);
		}else{						//changes the appropriate points
			tmp += half;
			points_[tmp-1].setFillColor(color_blue_);
			points_[tmp].setFillColor(color_red_);
		}
	}
}

void Score::
reset(){
	for(int i = 0; i < max_points_; ++i){
		if(i < max_points_ / 2){

			points_[i].setFillColor(color_blue_);
		}else{
			points_[i].setFillColor(color_red_);
		}
	}
}

int Score::
getMaxPoints(){
	return max_points_;
}

}; //namespace MoB