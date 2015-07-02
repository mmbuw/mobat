#include "Recognized_Token_Object.h"

#include <iostream>
namespace TTT {

Recognized_Token_Object::
Recognized_Token_Object(unsigned in_id, 
						glm::vec2 in_position, 
						unsigned in_life_time_in_ms ) 
						: id_(in_id),
						  life_time_in_ms_(in_life_time_in_ms),
						  remaining_life_time_in_ms_(in_life_time_in_ms),
						  physical_position_(in_position) {
}

void Recognized_Token_Object::
Draw(sf::RenderWindow& canvas) const {
	//std::cout << "Drawing Token: \n";
	//std::cout << id_ << "Hz @ ";
	//std::cout << token_circle_shape_.getPosition().x << " ";
	//std::cout << token_circle_shape_.getPosition().y << "\n";
	canvas.draw(token_circle_shape_);
}

void Recognized_Token_Object::
Recalculate_Geometry() {
	float radius = 0.02f * pixel_per_projection_;
	token_circle_shape_.setPosition(to_projection_space(physical_position_, radius));
	token_circle_shape_.setRadius(radius);
}

void Recognized_Token_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	token_circle_shape_.setFillColor(in_fill_color);
}

void Recognized_Token_Object::
Update_Alpha_Value(float in_alpha) {
	sf::Color new_color = token_circle_shape_.getFillColor();
	new_color.a = sf::Uint8(255.0 * in_alpha);

	token_circle_shape_.setFillColor(new_color);
}


void Recognized_Token_Object::
Set_Life_Time(unsigned in_life_time_in_ms) {
	life_time_in_ms_ = in_life_time_in_ms;
}

bool Recognized_Token_Object::
Update_Token(bool in_was_recognized, 
			 unsigned in_passed_time_in_ms, 
			 glm::vec2 const& in_position) {

		//std::cout << "Token Update, Decrease Life Time: " << !in_was_recognized << "\n";

		//reset life time if token was recognized
		if(in_was_recognized) {
			remaining_life_time_in_ms_ = life_time_in_ms_;
			Update_Alpha_Value(1.0);
			physical_position_ = in_position;
			return true;
		}

		//life time would be reduced to zero -> issue deletion
		if(in_passed_time_in_ms >= remaining_life_time_in_ms_) {
			return false;
		}

		remaining_life_time_in_ms_-=in_passed_time_in_ms;

		//std::cout << "Remaining life time for token with id " << id_ << "   ms: " << remaining_life_time_in_ms_;
		Update_Alpha_Value(remaining_life_time_in_ms_/(float)life_time_in_ms_);
		return true;
}

unsigned Recognized_Token_Object::
Get_Id() const {
	return id_;
}


sf::CircleShape Recognized_Token_Object::get_Circle() const{
	return token_circle_shape_;
}


}; //namespace TTT
