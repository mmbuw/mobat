#include "score.h"

#include <iostream>


namespace TTT {

Score::Score(glm::vec2 const& min, glm::vec2 const& max){
	std::cout<<"min:  "<<min.x<<"  "<<min.y<<"\n";
	std::cout<<"max:  "<<max.x<<"  "<<max.y<<"\n";

	sf::Vector2f size{pixel_projection_size_.y / 12 , pixel_projection_size_.y / 12};
	// sf::Vector2f size{max.y - min.y , max.y - min.y};
	if(up_.x == 0){//spiefeld vertikal
	float distance = (max.x - min.x) / ( 2 * points_.size() - 1); 
		for(unsigned i = 0; i < points_.size(); ++i){
			if(i < 3){
				points_[i].setFillColor(sf::Color::Blue);
			}else{
				points_[i].setFillColor(sf::Color::Red);
			}

			
			points_[i].setPosition(min.x + distance * i * 2, min.y - size.x/2);	//if table is vertical
			points_[i].setSize(size);
		}
	}else{	//spielfeld horizontal
	 float distance = (max.y - min.y) / (points_.size()) - size.x; 
		for(unsigned i = 0; i < points_.size(); ++i){
			if(i < 3){
				points_[i].setFillColor(sf::Color::Blue);
			}else{
				points_[i].setFillColor(sf::Color::Red);
			}

			points_[i].setPosition((max.x + min.x) / 2 - size.x/2, min.y + size.y/2 +i*distance * 2);  // if table is horizontal
			// points_[i].setPosition((max.x + min.x) / 2 - size.x/2, min.y + min.x);  // if table is horizontal
			points_[i].setSize(size);
		}
	}
}

void Score::
Draw(sf::RenderWindow& canvas) const {
	for(auto const& i : points_){

		canvas.draw(i);
	}
}

void Score::
update(int r_goals, int l_goals) {
	if(r_goals == l_goals)
	{
		points_[2].setFillColor(sf::Color::Blue);
		points_[3].setFillColor(sf::Color::Red);
	}else{
		int tmp = (r_goals - l_goals);

		if(tmp == -3){
			points_[0].setFillColor(sf::Color::Red);
		}else if(tmp == 3){
			points_[5].setFillColor(sf::Color::Blue);
		}else{
			if(tmp < 0){
				tmp += 3;
				points_[tmp-1].setFillColor(sf::Color::Blue);
				points_[tmp].setFillColor(sf::Color::Red);
			}else{

				tmp += 2;
				points_[tmp].setFillColor(sf::Color::Blue);
				points_[tmp+1].setFillColor(sf::Color::Red);
			}

		}

	}
	

}

void Score::
reset(){
	for(unsigned i = 0; i < points_.size(); ++i){
		if(i < 3){

			points_[i].setFillColor(sf::Color::Blue);
		}else{
			points_[i].setFillColor(sf::Color::Red);
		}
	}
}



}; //namespace TTT
