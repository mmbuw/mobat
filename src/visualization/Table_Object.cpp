#include "Table_Object.h"
#include <iostream>
namespace TTT {

void Table_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(table_rectangle_shape_);
}

void Table_Object::
Set_Physical_Size(sf::Vector2f const& in_physical_size) {


	physical_table_size_ = in_physical_size;
}

void Table_Object::
Recalculate_Geometry() {
	//ensure 5% of border at each side
	sf::Vector2f res_with_margin = //resolution;
				sf::Vector2f(resolution_.x - 0.1*resolution_.x,
							 resolution_.y - 0.1*resolution_.y);

	pixel_per_meter_ = std::min(res_with_margin.x / physical_table_size_.x, 
								res_with_margin.y / physical_table_size_.y);

	table_dims_in_px_  = physical_table_size_ * pixel_per_meter_;

	table_rectangle_shape_.setSize(table_dims_in_px_);
	table_rectangle_shape_.setPosition( (resolution_.x - table_dims_in_px_.x)/2.0,
									    (resolution_.y - table_dims_in_px_.y)/2.0
									 );

}

void Table_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	table_rectangle_shape_.setFillColor(in_fill_color);
}


sf::Vector2f Table_Object::
Physical_Size() const {
	return physical_table_size_;
}


};