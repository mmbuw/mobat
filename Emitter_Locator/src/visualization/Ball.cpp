#include "Ball.h"

/*#include <iostream>
*/
namespace TTT {

Ball::
Ball(sf::Vector2f const& pos, double rad) :
							position_(pos),
							//pos_on_table_(position_.x* physical_table_size_.x / resolution_.x, position_.y * physical_table_size_.y / resolution_.y),
							rad_(rad),
							ball_(2*(std::min(physical_table_size_.x, 
				   physical_table_size_.y) / rad) * pixel_per_meter_),
							//ball_(rad),
							dir_(sf::Vector2f(-0.01,10) ),
							should_move_(true)
{}

void Ball::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(ball_);
}

void Ball::
Recalculate_Geometry() {
	ball_.setFillColor(sf::Color::Red);
	sf::Vector2f coordinate_system_origin_in_px 
		= sf::Vector2f( (resolution_.x - table_dims_in_px_.x)/2.0,
						 resolution_.y - 
						 ((resolution_.y - table_dims_in_px_.y)/2.0) );


	float radius //= rad_;
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

void Ball::
Set_Fill_Color(sf::Color const& in_fill_color) {
	ball_.setFillColor(in_fill_color);
}

void Ball::
setPosition(double x, double y) {
	//ball_.setPosition(x/ (physical_table_size_.x / resolution_.x), y/ (physical_table_size_.y / resolution_.y));
	position_.x = x ;/// (physical_table_size_.x / resolution_.x);
	position_.y = y;//(physical_table_size_.y - y) ;/// (physical_table_size_.x / resolution_.x);


	Recalculate_Geometry();
/*	sf::Vector2f coordinate_system_origin_in_px 
		= sf::Vector2f( (resolution_.x - table_dims_in_px_.x)/2.0,
						 resolution_.y - 
						 ((resolution_.y - table_dims_in_px_.y)/2.0) );


	float token_radius 
		= 2*(std::min(physical_table_size_.x, 
				   physical_table_size_.y) / 50.0f) * pixel_per_meter_;  
	ball_
		.setPosition(coordinate_system_origin_in_px.x + x*pixel_per_meter_  - token_radius,
					 coordinate_system_origin_in_px.y - (y*pixel_per_meter_ + token_radius) );

	ball_.setRadius( token_radius );*/
}

sf::Vector2f Ball::getPosition() const{
	return ball_.getPosition()/*position_*/;
}

sf::Vector2f Ball::getTablePos() const{
	return position_;
}

double Ball::getRadius() const{
	return rad_;
}







}; //namespace TTT
