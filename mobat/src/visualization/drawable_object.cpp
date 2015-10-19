#include "drawable_object.hpp"
#include <iostream>

namespace MoB {

glm::vec2 DrawableObject::physical_table_size_ = glm::vec2(0.0f, 0.0f);
glm::vec2 DrawableObject::resolution_ = glm::vec2(0,0);
glm::vec2 DrawableObject::up_ = glm::vec2(0,0);
glm::vec2 DrawableObject::right_ = glm::vec2(0,0);
glm::vec2 DrawableObject::pixel_projection_offset_ = glm::vec2(0,0);
glm::vec2 DrawableObject::physical_projection_offset_ = glm::vec2(0,0);
glm::vec2 DrawableObject::physical_projection_size_ = glm::vec2(0,0);
glm::vec2 DrawableObject::pixel_projection_size_ = glm::vec2(0,0);
float DrawableObject::pixel_per_projection_ = 0.0f;
float DrawableObject::line_thickness_ = 0.0f;

DrawableObject::DrawableObject()
{};

DrawableObject::~DrawableObject()
{};

void DrawableObject::setBasis(glm::vec2 const& right, glm::vec2 const& up) {
  right_ = right;
  up_ = up;

}

void DrawableObject::setProjection(glm::vec2 const& origin, glm::vec2 const& size) {
  physical_projection_offset_ = origin;
  physical_projection_size_ = size;
  glm::vec2 size2 = right_ * size.x + up_ * size.y;
  if(size2.x > size2.y) {
    pixel_per_projection_ = resolution_.x / size2.x;
    pixel_projection_offset_ = {0, (resolution_.y - size2.y * pixel_per_projection_ ) * 0.5f};
  } else {
    pixel_per_projection_ = resolution_.y / size2.y;
    pixel_projection_offset_ = {(resolution_.x - size2.x * pixel_per_projection_ ) * 0.5f, 0};
  }

  pixel_projection_size_ = size2 * pixel_per_projection_;
}

void DrawableObject::setResolution(glm::vec2 const& res) {
  resolution_ = res;   
  line_thickness_ = std::min(resolution_.x, resolution_.y) * 0.007;
}

sf::Vector2f DrawableObject::toProjectionSpace(glm::vec2 pos, float radius) {
  pos -= physical_projection_offset_;
  pos *= pixel_per_projection_;
  // add margin
  pos = right_ * pos.x + up_ * pos.y;
  pos += pixel_projection_offset_;
  if((up_ + right_).y > 0) {
    pos.y = resolution_.y - pos.y;
  }
  if((up_ + right_).x > 0) {
    pos.x = resolution_.x - pos.x;
  }
  pos -= glm::vec2{radius, radius};
  
  return toSf(pos);
} 

sf::Vector2f DrawableObject::toProjectionSize(glm::vec2 size) {
  if((up_ + right_).y < 0) {
    if((up_ + right_).x < 0) {
      size = right_ * size.x + up_ * size.y;
    } else {
      size = right_ * -size.x + up_ * size.y;
    }
  } else {
    if((up_ + right_).x < 0) {
      size = right_ * size.x + up_ * -size.y;
    } else {
      size = right_ * -size.x + up_ * -size.y;
    }
  }

  return sf::Vector2f{size.x * pixel_per_projection_, size.y * pixel_per_projection_};
}

glm::vec2 DrawableObject::pixelProjectionOffset() {
  return pixel_projection_offset_;
}
glm::vec2 DrawableObject::pixelProjectionSize() {
  return pixel_projection_size_;
}

void DrawableObject::setPhysTableSize(glm::vec2 const& size) {
  physical_table_size_ = size;
}

sf::Vector2f toSf(glm::vec2 const& vec) {
  return sf::Vector2f{vec.x, vec.y};
}

sf::Color toSf(glm::vec3 const& vec) {
  return sf::Color{sf::Uint8(vec.x * 255), sf::Uint8(vec.y * 255), sf::Uint8(vec.z * 255)};
}

glm::vec2 toGlm(sf::Vector2f const& vec) {
  return glm::vec2{vec.x, vec.y};
}


};