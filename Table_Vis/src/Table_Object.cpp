#include "../include/Table_Object.h"

namespace TTT {

void Table_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(table_rectangle_shape_);
}

void Table_Object::
Set_Physical_Size(sf::Vector2f const& in_physical_size) {
	physical_size_ = in_physical_size;
}

void Table_Object::
Recalculate_Table_Geometry(sf::Vector2f const& resolution) {

	//ensure 5% of border at each side
	sf::Vector2f res_with_margin = 
				sf::Vector2f(resolution.x - 0.1*resolution.x,
							 resolution.y - 0.1*resolution.y);

	float pixel_per_meter = std::min(res_with_margin.x / physical_size_.x, 
									 res_with_margin.y / physical_size_.y);

	sf::Vector2f table_dims_in_px  = physical_size_ * pixel_per_meter;

	table_rectangle_shape_.setSize(table_dims_in_px);
	table_rectangle_shape_.setPosition( 0.05*resolution.x,
									   0.05*resolution.y
									 );
}

sf::Vector2f
Table_Object::
Physical_Table_Size() const {
	return physical_size_;
}

};