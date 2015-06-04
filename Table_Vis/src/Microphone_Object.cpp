#include "../include/Microphone_Object.h"

namespace TTT {

void Microphone_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(microphone_circle_shape_);
}

void Microphone_Object::
Set_Position(Vec2f const& in_pos) {
	pos_ = in_pos;
}

Vec2f Microphone_Object::
Position() const {
	return pos_;
}



};