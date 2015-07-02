#include "Microphone_Object.h"
#include <iostream>

namespace TTT {

Microphone_Object::
Microphone_Object(unsigned in_id, glm::vec2 const& in_position ) {
	id_ = in_id;
	physical_position_ = in_position;
}

void Microphone_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(microphone_circle_shape_);
}

void Microphone_Object::
Recalculate_Geometry() {
	float radius = 0.02f * pixel_per_projection_;  
	microphone_circle_shape_.setPosition(to_projection_space(physical_position_, radius));
	microphone_circle_shape_.setRadius(radius);
}

void Microphone_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	microphone_circle_shape_.setFillColor(in_fill_color);
}

};