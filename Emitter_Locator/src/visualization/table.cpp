#include "table.hpp"
#include "configurator.hpp"
#include <iostream>

namespace TTT {

void Table::
draw(sf::RenderWindow& canvas) const {
  canvas.draw(table_rectangle_shape_);
  canvas.draw(middle_);
  canvas.draw(middle_line1_);
  canvas.draw(middle_line2_);
  canvas.draw(right_goal_);
  canvas.draw(left_goal_);
  canvas.draw(right_line_);
  canvas.draw(left_line_);

  canvas.draw(error_vis_);
}

void Table::
draw(sf::RenderWindow& canvas, std::vector<Microphone> const& microphones, glm::vec4 const& toas) const {

  //sf::Shader error_vis_shader;
/*
  if(!error_vis_shader.loadFromFile("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag")) {
  //  std::cout << "Did not load shaders succesfully.\n";
  } else {
  //  std::cout << "=)\n";
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

void Table::generateErrorGrid() {
  std::array<glm::vec2, 4> mics{ 
    configurator().getVec("microphone1_pos"),
    configurator().getVec("microphone2_pos"),
    configurator().getVec("microphone3_pos"),
    configurator().getVec("microphone4_pos")
  };

  glm::vec2 min{std::min(std::min(std::min(mics[0].x , mics[1].x), mics[2].x), mics[3].x),
                std::min(std::min(std::min(mics[0].y , mics[1].y), mics[2].y), mics[3].y)};
  glm::vec2 max{std::max(std::max(std::max(mics[0].x , mics[1].x), mics[2].x), mics[3].x),
                std::max(std::max(std::max(mics[0].y , mics[1].y), mics[2].y), mics[3].y)};

  grid_dimensions_ = glm::vec2{(max.x - min.x), (max.y - min.y)};
  grid_dimensions_ /= configurator().getFloat("sample_size");
  grid_dimensions_ = glm::floor(grid_dimensions_);

  glm::vec2 step{physical_table_size_ / grid_dimensions_};

  error_vis_ = sf::VertexArray(sf::Quads, 4 * grid_dimensions_.x * grid_dimensions_.y);
  for (std::size_t x = 0; x < grid_dimensions_.x; ++x) {
    for (std::size_t y = 0; y < grid_dimensions_.y; ++y) {
      glm::vec2 origin{x * step.x, y * step.y};
      std::size_t base_index = (x * grid_dimensions_.y + y) * 4; 

      error_vis_[base_index].position     = sf::Vector2f{toProjectionSpace(origin)};
      error_vis_[base_index + 1].position = sf::Vector2f{toProjectionSpace(origin + glm::vec2{0.0f, step.y})};
      error_vis_[base_index + 2].position = sf::Vector2f{toProjectionSpace(origin + step)};
      error_vis_[base_index + 3].position = sf::Vector2f{toProjectionSpace(origin + glm::vec2{step.x, 0.0f})};

      error_vis_[base_index].color     = sf::Color::Red;
      error_vis_[base_index + 1].color = sf::Color::Red;
      error_vis_[base_index + 2].color = sf::Color::Red;
      error_vis_[base_index + 3].color = sf::Color::Red;
    }
  }
}

void Table::setErrorDistribution(std::vector<std::vector<float>> dist) {

  float min_error = std::numeric_limits<float>::max();
  for (std::size_t x = 0; x < grid_dimensions_.x; ++x) {
    for (std::size_t y = 0; y < grid_dimensions_.y; ++y) {
      if (dist[x][y] < min_error) min_error = dist[x][y];
    }
  }

  float max_error = 0;
  for (std::size_t x = 0; x < grid_dimensions_.x; ++x) {
    for (std::size_t y = 0; y < grid_dimensions_.y; ++y) {
      if (dist[x][y] > max_error) max_error = dist[x][y];
    }
  }

  for (std::size_t x = 0; x < grid_dimensions_.x; ++x) {
    for (std::size_t y = 0; y < grid_dimensions_.y; ++y) {
      std::size_t base_index = (x * grid_dimensions_.y + y) * 4; 
    
      float normalized_error = (dist[x][y] - min_error) / (max_error - min_error);
      sf::Color color{sf::Uint8(normalized_error * 255), sf::Uint8((1.0f -normalized_error) * 255),0}; 
      error_vis_[base_index].color = color;
      error_vis_[base_index + 1].color = color;
      error_vis_[base_index + 2].color = color;
      error_vis_[base_index + 3].color = color;
    }
  }
}

void Table::
recalculateGeometry() {

  table_rectangle_shape_.setSize(toProjectionSize(physical_table_size_));
  table_rectangle_shape_.setPosition(toProjectionSpace(glm::vec2{0.0f}));
  generateErrorGrid();

  middle_.setRadius(pixel_per_projection_ * 0.1f);
  middle_.setPosition(toProjectionSpace(glm::vec2{physical_projection_offset_ + physical_projection_size_ * 0.5f}, middle_.getRadius()));
  middle_.setOutlineThickness(line_thickness_);
  middle_.setOutlineColor(sf::Color::Yellow);
  middle_.setFillColor(sf::Color::Transparent);
  
  middle_line1_.setFillColor(sf::Color::Yellow);
  middle_line1_.setSize(toProjectionSize(glm::vec2{physical_projection_size_.x * 0.5f - middle_.getRadius() / pixel_per_projection_, line_thickness_ / pixel_per_projection_}));
  middle_line1_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{0.0f, (physical_projection_size_.y - line_thickness_ / pixel_per_projection_) * 0.5f}));

  middle_line2_.setFillColor(sf::Color::Yellow);
  middle_line2_.setSize(toProjectionSize(glm::vec2{physical_projection_size_.x * 0.5f - middle_.getRadius() / pixel_per_projection_, line_thickness_ / pixel_per_projection_}));
  middle_line2_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{physical_projection_size_.x * 0.5f + middle_.getRadius() / pixel_per_projection_, (physical_projection_size_.y - line_thickness_ / pixel_per_projection_) * 0.5f}));

  left_line_.setFillColor(sf::Color::Yellow);
  left_line_.setSize(toProjectionSize(glm::vec2{physical_projection_size_.x, line_thickness_ / pixel_per_projection_}));
  left_line_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{0.0f, physical_projection_size_.y * 0.35f - line_thickness_ / pixel_per_projection_ * 0.5f}));

  right_line_.setFillColor(sf::Color::Yellow);
  right_line_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{0.0f, physical_projection_size_.y * 0.65f - line_thickness_ / pixel_per_projection_ * 0.5f}));
  right_line_.setSize(toProjectionSize(glm::vec2{physical_projection_size_.x, line_thickness_ / pixel_per_projection_}));

  left_goal_.setRadius(pixel_per_projection_ * 0.25f);
  left_goal_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{physical_projection_size_.x * 0.5f, - left_goal_.getRadius() / pixel_per_projection_ * 0.5f}, left_goal_.getRadius()));
  left_goal_.setOutlineThickness(line_thickness_);
  left_goal_.setOutlineColor(sf::Color::Yellow);
  left_goal_.setFillColor(sf::Color::Transparent);

  right_goal_.setRadius(pixel_per_projection_ * 0.25f);
  right_goal_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{physical_projection_size_.x * 0.5f,physical_projection_size_.y + left_goal_.getRadius() / pixel_per_projection_ * 0.5f}, left_goal_.getRadius()));
  right_goal_.setOutlineThickness(line_thickness_);
  right_goal_.setOutlineColor(sf::Color::Yellow);
  right_goal_.setFillColor(sf::Color::Transparent);
}

void Table::
setFillColor(sf::Color const& in_fill_color) {
  
  table_rectangle_shape_.setFillColor(in_fill_color);
}

};