#include "microphone.hpp"
#include <iostream>

namespace TTT {

Microphone::
Microphone(unsigned in_id, glm::vec2 const& in_position ) {
	id_ = in_id;
	physical_position_ = in_position;
}

void Microphone::
draw(sf::RenderWindow& canvas) const {
	canvas.draw(microphone_circle_shape_);
}

void Microphone::
recalculateGeometry() {
	float radius = 10.0f;  
	microphone_circle_shape_.setPosition(toProjectionSpace(physical_position_, radius));
	microphone_circle_shape_.setRadius(radius);
}

void Microphone::
setFillColor(sf::Color const& in_fill_color) {
	microphone_circle_shape_.setFillColor(in_fill_color);
}

};