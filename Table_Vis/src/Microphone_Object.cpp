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
Recalculate_Geometry(sf::Vector2f const& resolution, 
					 sf::Vector2f const& physical_table_size) {

	sf::Vector2f res_with_margin = //resolution;
				sf::Vector2f(resolution.x - 0.1*resolution.x,
							 resolution.y - 0.1*resolution.y);

	float pixel_per_meter = std::min(res_with_margin.x / physical_table_size.x, 
									 res_with_margin.y / physical_table_size.y);

	sf::Vector2f table_dims_in_px  = physical_table_size * pixel_per_meter;

	//table_rectangle_shape_.setSize(table_dims_in_px);
	//table_rectangle_shape_.setPosition( (resolution.x - table_dims_in_px.x)/2.0,
	//								    (resolution.y - table_dims_in_px.y)/2.0
	//								 );

	sf::Vector2f coordinate_system_origin_in_px 
		= sf::Vector2f( (resolution.x - table_dims_in_px.x)/2.0,
						 resolution.y - 
						 ((resolution.y - table_dims_in_px.y)/2.0) );


	float microphone_radius 
		= (std::min(physical_table_size.x, 
				   physical_table_size.y) / 50.0f) * pixel_per_meter;  
	microphone_circle_shape_
		.setPosition(coordinate_system_origin_in_px.x + physical_position_.x*pixel_per_meter - microphone_radius,
					 coordinate_system_origin_in_px.y - (physical_position_.y*pixel_per_meter+microphone_radius) );

	microphone_circle_shape_.setRadius( microphone_radius );
}

void Microphone_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	microphone_circle_shape_.setFillColor(in_fill_color);
}





};