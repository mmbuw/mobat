#include "score.h"



namespace TTT {

Score::Score(double x_max, double y_max){
	sf::Vector2f size{20,20};
	for(unsigned i = 0; i < points_.size(); ++i){
		if(i < 3){

			points_[i].setFillColor(sf::Color::Blue);
		}else{
			points_[i].setFillColor(sf::Color::Red);
		}

		// points_[i].setPosition(x_max /2, (i)*90);  // if table is horizontal
		points_[i].setPosition(x_max + (i)*50, y_max/2);	//if table is vertical
		points_[i].setSize(size);


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
