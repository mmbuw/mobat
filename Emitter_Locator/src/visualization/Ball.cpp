#include "Ball.h"

/*#include <iostream>
*/
namespace TTT {

Ball::
Ball(sf::Vector2f const& pos, double rad) :
							ball_(rad),
							dir_(sf::Vector2f(-0.01,10))
{
	ball_.setPosition(pos);
}

void Ball::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(ball_);
}


sf::CircleShape Ball::get_Circle() const{
	return ball_;
}


void Ball::set_dir(sf::Vector2f const& dir){
	dir_ = dir;
}

void Ball::
Set_Fill_Color(sf::Color const& in_fill_color) {
	ball_.setFillColor(in_fill_color);
}

void Ball::
setPosition(double x, double y) {
	ball_.setPosition(x - ball_.getRadius(), y - ball_.getRadius());
}

sf::Vector2f Ball::getPosition() const{
	return ball_.getPosition() + sf::Vector2f(ball_.getRadius(), ball_.getRadius());
}

double Ball::getRadius() const{
	return ball_.getRadius();
}







}; //namespace TTT
