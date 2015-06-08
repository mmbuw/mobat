#include "../include/Recognized_Token_Object.h"

namespace TTT {

Recognized_Token_Object::
Recognized_Token_Object(unsigned in_id, 
						sf::Vector2f in_position, 
						unsigned in_life_time_in_ms ) 
						: id_(in_id),
						  physical_position_(in_position),
						  life_time_in_ms_(in_life_time_in_ms),
						  remaining_life_time_in_ms_(in_life_time_in_ms) {
}

void Recognized_Token_Object::
Draw(sf::RenderWindow& canvas) const {

	canvas.draw(token_circle_shape_);
}

void Recognized_Token_Object::
Recalculate_Geometry(sf::Vector2f const& resolution, 
					 sf::Vector2f const& physical_table_size) {

	sf::Vector2f res_with_margin = //resolution;
				sf::Vector2f(resolution.x - 0.1*resolution.x,
							 resolution.y - 0.1*resolution.y);

	float pixel_per_meter = std::min(res_with_margin.x / physical_table_size.x, 
									 res_with_margin.y / physical_table_size.y);

	sf::Vector2f table_dims_in_px  = physical_table_size * pixel_per_meter;

	sf::Vector2f coordinate_system_origin_in_px 
		= sf::Vector2f( (resolution.x - table_dims_in_px.x)/2.0,
						 resolution.y - 
						 ((resolution.y - table_dims_in_px.y)/2.0) );


	float token_radius 
		= (std::min(physical_table_size.x, 
				   physical_table_size.y) / 50.0f) * pixel_per_meter;  
	token_circle_shape_
		.setPosition(coordinate_system_origin_in_px.x + physical_position_.x*pixel_per_meter  - token_radius,
					 coordinate_system_origin_in_px.y - (physical_position_.y*pixel_per_meter + token_radius) );

	token_circle_shape_.setRadius( token_radius );
}

void Recognized_Token_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	token_circle_shape_.setFillColor(in_fill_color);
}

void Recognized_Token_Object::
Update_Alpha_Value(float in_alpha) {
	sf::Color new_color = token_circle_shape_.getFillColor();
	new_color.a = sf::Uint8(255 * in_alpha);

	token_circle_shape_.setFillColor(new_color);
}


void Recognized_Token_Object::
Set_Life_Time(unsigned in_life_time_in_ms) {
	remaining_life_time_in_ms_ = life_time_in_ms_ = in_life_time_in_ms;
}

bool Recognized_Token_Object::
Update_Life_Time(bool in_was_recognized, unsigned in_passed_time_in_ms) {

		//reset life time if token was recognized
		if(in_was_recognized) {
			remaining_life_time_in_ms_ = life_time_in_ms_;
			Update_Alpha_Value(1.0);
			return true;
		}

		//life time would be reduced to zero -> issue deletion
		if(in_passed_time_in_ms >= remaining_life_time_in_ms_) {
			return false;
		}

		remaining_life_time_in_ms_-=in_passed_time_in_ms;
		Update_Alpha_Value(remaining_life_time_in_ms_/life_time_in_ms_);
		return true;
}

unsigned Recognized_Token_Object::
Get_Id() const {
	return id_;
}

}; //namespace TTT
