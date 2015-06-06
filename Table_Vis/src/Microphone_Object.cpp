#include "../include/Microphone_Object.h"

namespace TTT {

void Microphone_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(microphone_circle_shape_);
}

void Microphone_Object::
Set_Position(sf::Vector2f const& in_pos) {
	pos_ = in_pos;
}

sf::Vector2f Microphone_Object::
Position() const {
	return pos_;
}


};