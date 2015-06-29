#include "points.h"

/*#include <iostream>
*/
namespace TTT {

Points::
Points(){
	for(unsigned i = 0; i < points_.size(); ++i){
		auto x = points_[i];
		if(i < 3){
			x.setFillColor(sf::Color::Blue);
		}else{
			x.setFillColor(sf::Color::Red);
		}

		x.setPosition(physical_table_size_.x / i, physical_table_size_.y/2);


	}
}

void Points::
Draw(sf::RenderWindow& canvas) const {
	for(auto const& i : points_){

		canvas.draw(i);
	}
}

void Points::
update(int r_goals, int l_goals) {
	if(r_goals == l_goals)
	{
		points_[2].setFillColor(sf::Color::Blue);
		points_[3].setFillColor(sf::Color::Red);
	}else{
		int tmp = (r_goals - l_goals);
		if(tmp == -6){
			points_[0].setFillColor(sf::Color::Red);
		}else if(tmp == 6){
			points_[5].setFillColor(sf::Color::Blue);
		}else{
			tmp = 3 + tmp/2;
			if(tmp < 3){
				points_[tmp-1].setFillColor(sf::Color::Blue);
				points_[tmp].setFillColor(sf::Color::Red);
			}else{
				points_[tmp-2].setFillColor(sf::Color::Blue);
				points_[tmp-1].setFillColor(sf::Color::Red);
			}
		}

	}
	

}



}; //namespace TTT
