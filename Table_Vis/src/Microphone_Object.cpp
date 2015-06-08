#include "../include/Microphone_Object.h"

namespace TTT {

Microphone_Object::
Microphone_Object(unsigned in_id, sf::Vector2f in_position ) {
	id_ = in_id;
	physical_position_ = in_position;
}

void Microphone_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(microphone_circle_shape_);
}

void Microphone_Object::
Recalculate_Geometry() {

	sf::Vector2f coordinate_system_origin_in_px 
		= sf::Vector2f( (resolution_.x - table_dims_in_px_.x)/2.0,
						 resolution_.y - 
						 ((resolution_.y - table_dims_in_px_.y)/2.0) );

	float microphone_radius 
		= (std::min(physical_table_size_.x, 
				    physical_table_size_.y) / 50.0f) * pixel_per_meter_;  
	microphone_circle_shape_
		.setPosition(coordinate_system_origin_in_px.x + physical_position_.x*pixel_per_meter_ - microphone_radius,
					 coordinate_system_origin_in_px.y - (physical_position_.y*pixel_per_meter_+microphone_radius) );

	microphone_circle_shape_.setRadius( microphone_radius );
}

void Microphone_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	microphone_circle_shape_.setFillColor(in_fill_color);
}





};