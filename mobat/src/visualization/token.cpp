#include "token.hpp"

#include <iostream>
namespace MoB {

Token::
Token(unsigned in_id, 
						glm::vec2 in_position, 
						unsigned in_life_time_in_ms ) 
						: id_(in_id),
						  life_time_in_ms_(in_life_time_in_ms),
						  remaining_life_time_in_ms_(in_life_time_in_ms),
						  physical_position_(in_position) {
	token_circle_shape_.setOutlineThickness(10);
	token_circle_shape_.setOutlineColor(sf::Color(0, 0, 0, 255.0));
}

void Token::
draw(sf::RenderWindow& canvas) const {
	//std::cout << "Drawing Token: \n";
	//std::cout << id_ << "Hz @ ";
	//std::cout << token_circle_shape_.getPosition().x << " ";
	//std::cout << token_circle_shape_.getPosition().y << "\n";
	canvas.draw(token_circle_shape_);
	// std::cout<<"getting drawed\n";
}

void Token::
recalculateGeometry() {
	float radius = 0.04* pixel_per_projection_;
	token_circle_shape_.setPosition(toProjectionSpace(physical_position_, radius));
	token_circle_shape_.setRadius(radius);
}

void Token::
setFillColor(sf::Color const& in_fill_color) {
	token_circle_shape_.setFillColor(in_fill_color);
}

void Token::
updateAlphaValue(float in_alpha) {
	sf::Color new_color = token_circle_shape_.getFillColor();
	new_color.a = sf::Uint8(255.0 * in_alpha);

	token_circle_shape_.setFillColor(new_color);

	token_circle_shape_.setOutlineColor(sf::Color(0, 0, 0, 255.0*in_alpha));
}


void Token::
setLifeTime(unsigned in_life_time_in_ms) {
	life_time_in_ms_ = in_life_time_in_ms;
}

bool Token::
update(bool in_was_recognized, 
			 unsigned in_passed_time_in_ms, 
			 glm::vec2 const& in_position) {

		//std::cout << "Token Update, Decrease Life Time: " << !in_was_recognized << "\n";

		//reset life time if token was recognized
		if(in_was_recognized) {
			remaining_life_time_in_ms_ = life_time_in_ms_;
			updateAlphaValue(1.0);
			physical_position_ = in_position;
			return true;
		}

		//life time would be reduced to zero -> issue deletion
		if(in_passed_time_in_ms >= remaining_life_time_in_ms_) {
			return false;
		}

		remaining_life_time_in_ms_-=in_passed_time_in_ms;

		//std::cout << "Remaining life time for token with id " << id_ << "   ms: " << remaining_life_time_in_ms_;
		updateAlphaValue(remaining_life_time_in_ms_/(float)life_time_in_ms_);
		return true;
}

unsigned Token::
getID() const {
	return id_;
}


sf::CircleShape Token::getCircle() const{
	return token_circle_shape_;
}

glm::vec2 const& Token::getPhysicalPosition() const {
	return physical_position_;
}



}; //namespace MoB
