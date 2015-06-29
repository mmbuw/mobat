#include "Table_Object.h"
#include <iostream>
namespace TTT {

void Table_Object::
Draw(sf::RenderWindow& canvas) const {
		canvas.draw(table_rectangle_shape_);
}

void Table_Object::
Draw(sf::RenderWindow& canvas, std::vector<Microphone_Object> const& microphones, glm::vec4 const& toas) const {

	sf::Shader error_vis_shader;

	if(!error_vis_shader.loadFromFile("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag")) {
	//	std::cout << "Did not load shaders succesfully.\n";
	} else {
	//	std::cout << "=)\n";
	}

	error_vis_shader.setParameter("mic1_pos", microphones[0].physical_position_ );
	error_vis_shader.setParameter("mic2_pos", microphones[1].physical_position_ );
	error_vis_shader.setParameter("mic3_pos", microphones[2].physical_position_ );
	error_vis_shader.setParameter("mic4_pos", microphones[3].physical_position_ );

	//std::cout << "toas in draw call: " << toas[0] << ", " << toas[1] << ", " << toas[2] << ", " << toas[3] << "\n";

	error_vis_shader.setParameter("toas", toas[0], toas[1], toas[2], toas[3]);

	//sf::Shader::bind(&error_vis_shader);
		canvas.draw(table_rectangle_shape_);
	sf::Shader::bind(NULL);
}

void Table_Object::
Set_Physical_Size(glm::vec2 const& in_physical_size) {


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

	table_rectangle_shape_.setSize(sf::Vector2f{table_dims_in_px_.x, table_dims_in_px_.y});
	table_rectangle_shape_.setPosition((resolution_.x - table_dims_in_px_.x)/2.0,
									    (resolution_.y - table_dims_in_px_.y)/2.0);

}

void Table_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	table_rectangle_shape_.setFillColor(in_fill_color);
}


glm::vec2 Table_Object::
Physical_Size() const {
	return physical_table_size_;
}


};