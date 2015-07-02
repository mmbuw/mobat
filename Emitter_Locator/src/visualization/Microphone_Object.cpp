#include "Microphone_Object.h"

namespace TTT {

Microphone_Object::
Microphone_Object(unsigned in_id, sf::Vector2f const& in_position ) {
	id_ = in_id;
	physical_position_ = in_position;
}

void Microphone_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(microphone_circle_shape_);
}

void Microphone_Object::
Recalculate_Geometry() {

	float microphone_radius 
		= (std::min(physical_table_size_.x, 
				    physical_table_size_.y) / 50.0f) * pixel_per_meter_;  
	microphone_circle_shape_
		.setPosition(pixel_table_offset_.x + physical_position_.x * pixel_per_meter_ - microphone_radius,
					 pixel_table_offset_.y + physical_position_.y * pixel_per_meter_ + microphone_radius);

	microphone_circle_shape_.setRadius(microphone_radius);
}

void Microphone_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	microphone_circle_shape_.setFillColor(in_fill_color);
}

};