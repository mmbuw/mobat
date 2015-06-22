#include "Ball.h"

/*#include <iostream>
*/
namespace TTT {

Ball::
Ball(sf::Vector2f const& pos, double rad) :
							position_(pos),
							rad_(rad),
							ball_(rad),
							dir_(sf::Vector2f(-0.01,10) )
{}

void Ball::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(ball_);
}

void Ball::
Recalculate_Geometry() {
	sf::Vector2f coordinate_system_origin_in_px 
		= sf::Vector2f( (resolution_.x - table_dims_in_px_.x)/2.0,
						 resolution_.y - 
						 ((resolution_.y - table_dims_in_px_.y)/2.0) );


	float radius 
		= 2*(std::min(physical_table_size_.x, 
				   physical_table_size_.y) / 50.0f) * pixel_per_meter_;  
	ball_
		.setPosition(coordinate_system_origin_in_px.x + position_.x*pixel_per_meter_  - radius,
					 coordinate_system_origin_in_px.y - (position_.y*pixel_per_meter_ + radius) );

	ball_.setRadius( radius );
}


sf::CircleShape Ball::get_Circle() const{
	return ball_;
}


void Ball::set_dir(sf::Vector2f const& dir){
	dir_ = dir;
}

}; //namespace TTT
