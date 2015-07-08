#include "Table_Object.h"
#include <iostream>

namespace TTT {

void Table_Object::
Draw(sf::RenderWindow& canvas) const {
	canvas.draw(table_rectangle_shape_);
	canvas.draw(middle_);
	canvas.draw(middle_line1_);
	canvas.draw(middle_line2_);
	canvas.draw(right_goal_);
	canvas.draw(left_goal_);
	canvas.draw(right_line_);
	canvas.draw(left_line_);
}

void Table_Object::
Draw(sf::RenderWindow& canvas, std::vector<Microphone_Object> const& microphones, glm::vec4 const& toas) const {

	//sf::Shader error_vis_shader;
/*
	if(!error_vis_shader.loadFromFile("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag")) {
	//	std::cout << "Did not load shaders succesfully.\n";
	} else {
	//	std::cout << "=)\n";
	}
*/
	// error_vis_shader.setParameter("mic1_pos", microphones[0].physical_position_ );
	// error_vis_shader.setParameter("mic2_pos", microphones[1].physical_position_ );
	// error_vis_shader.setParameter("mic3_pos", microphones[2].physical_position_ );
	// error_vis_shader.setParameter("mic4_pos", microphones[3].physical_position_ );

	//std::cout << "toas in draw call: " << toas[0] << ", " << toas[1] << ", " << toas[2] << ", " << toas[3] << "\n";

	//error_vis_shader.setParameter("toas", toas[0], toas[1], toas[2], toas[3]);

	//sf::Shader::bind(&error_vis_shader);
	canvas.draw(table_rectangle_shape_);

	//sf::Shader::bind(NULL);
}

void Table_Object::
Recalculate_Geometry() {
	table_rectangle_shape_.setSize(to_projection_size(physical_table_size_));
	table_rectangle_shape_.setPosition(to_projection_space(glm::vec2{0.0f}));



	middle_.setRadius(pixel_per_projection_ * 0.1f);
	middle_.setPosition(to_projection_space(glm::vec2{physical_projection_offset_ + physical_projection_size_ * 0.5f}, middle_.getRadius()));
	middle_.setOutlineThickness(line_thickness_);
	middle_.setOutlineColor(sf::Color::Yellow);
	middle_.setFillColor(sf::Color::Transparent);
	
	middle_line1_.setFillColor(sf::Color::Yellow);
	middle_line1_.setSize(to_projection_size(glm::vec2{physical_projection_size_.x * 0.5f - middle_.getRadius() / pixel_per_projection_, line_thickness_ / pixel_per_projection_}));
	middle_line1_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{0.0f, (physical_projection_size_.y - line_thickness_ / pixel_per_projection_) * 0.5f}));

	middle_line2_.setFillColor(sf::Color::Yellow);
	middle_line2_.setSize(to_projection_size(glm::vec2{physical_projection_size_.x * 0.5f - middle_.getRadius() / pixel_per_projection_, line_thickness_ / pixel_per_projection_}));
	middle_line2_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{physical_projection_size_.x * 0.5f + middle_.getRadius() / pixel_per_projection_, (physical_projection_size_.y - line_thickness_ / pixel_per_projection_) * 0.5f}));

	left_line_.setFillColor(sf::Color::Yellow);
	left_line_.setSize(to_projection_size(glm::vec2{physical_projection_size_.x, line_thickness_ / pixel_per_projection_}));
	left_line_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{0.0f, physical_projection_size_.y * 0.35f - line_thickness_ / pixel_per_projection_ * 0.5f}));

	right_line_.setFillColor(sf::Color::Yellow);
	right_line_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{0.0f, physical_projection_size_.y * 0.65f - line_thickness_ / pixel_per_projection_ * 0.5f}));
	right_line_.setSize(to_projection_size(glm::vec2{physical_projection_size_.x, line_thickness_ / pixel_per_projection_}));

	left_goal_.setRadius(pixel_per_projection_ * 0.25f);
	left_goal_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{physical_projection_size_.x * 0.5f, - left_goal_.getRadius() / pixel_per_projection_ * 0.5f}, left_goal_.getRadius()));
	left_goal_.setOutlineThickness(line_thickness_);
	left_goal_.setOutlineColor(sf::Color::Yellow);
	left_goal_.setFillColor(sf::Color::Transparent);

	right_goal_.setRadius(pixel_per_projection_ * 0.25f);
	right_goal_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{physical_projection_size_.x * 0.5f,physical_projection_size_.y + left_goal_.getRadius() / pixel_per_projection_ * 0.5f}, left_goal_.getRadius()));
	right_goal_.setOutlineThickness(line_thickness_);
	right_goal_.setOutlineColor(sf::Color::Yellow);
	right_goal_.setFillColor(sf::Color::Transparent);
}

void Table_Object::
Set_Fill_Color(sf::Color const& in_fill_color) {
	
	table_rectangle_shape_.setFillColor(in_fill_color);
}

};