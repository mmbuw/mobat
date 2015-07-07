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

	// error_vis_shader.setParameter("mic1_pos", microphones[0].physical_position_ );
	// error_vis_shader.setParameter("mic2_pos", microphones[1].physical_position_ );
	// error_vis_shader.setParameter("mic3_pos", microphones[2].physical_position_ );
	// error_vis_shader.setParameter("mic4_pos", microphones[3].physical_position_ );

	//std::cout << "toas in draw call: " << toas[0] << ", " << toas[1] << ", " << toas[2] << ", " << toas[3] << "\n";

	error_vis_shader.setParameter("toas", toas[0], toas[1], toas[2], toas[3]);

	//sf::Shader::bind(&error_vis_shader);
		canvas.draw(table_rectangle_shape_);
	sf::Shader::bind(NULL);
}

void Table_Object::
Recalculate_Geometry() {
	table_rectangle_shape_.setSize(to_projection_size(physical_table_size_));
	table_rectangle_shape_.setPosition(to_projection_space(glm::vec2{0.0f}));
}

void Table_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	table_rectangle_shape_.setFillColor(in_fill_color);
}

};