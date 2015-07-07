#include "Drawable_Object.h"
#include <iostream>

namespace TTT {

glm::vec2 Drawable_Object::physical_table_size_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::resolution_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::up_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::right_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::pixel_projection_offset_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::physical_projection_offset_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::physical_projection_size_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::pixel_projection_size_ = glm::vec2(0,0);
float Drawable_Object::pixel_per_projection_ = 0.0f;

Drawable_Object::Drawable_Object()
{};

Drawable_Object::~Drawable_Object()
{};

void Drawable_Object::set_basis(glm::vec2 const& right, glm::vec2 const& up) {
    right_ = right;
    up_ = up;

}

void Drawable_Object::set_projection(glm::vec2 const& origin, glm::vec2 const& size) {
    physical_projection_offset_ = origin;
    physical_projection_size_ = size;
    glm::vec2 size2 = right_ * size.x + up_ * size.y;
    if(size2.x > size2.y) {
        pixel_per_projection_ = resolution_.x / size2.x;
        pixel_projection_offset_ = {0, (resolution_.y - size2.y * pixel_per_projection_ ) * 0.5f};
    }
    else {
        pixel_per_projection_ = resolution_.y / size2.y;
        pixel_projection_offset_ = {(resolution_.x - size2.x * pixel_per_projection_ ) * 0.5f, 0};
    }

    pixel_projection_size_ = size2 * pixel_per_projection_;
}

void Drawable_Object::set_resolution(glm::vec2 const& res) {
    resolution_ = res;   
}

sf::Vector2f Drawable_Object::to_projection_space(glm::vec2 pos, float radius) {
    pos -= physical_projection_offset_;
    pos *= pixel_per_projection_;
    pos -= glm::vec2{radius, radius};
    // add margin
    pos = right_ * pos.x + up_ * pos.y;
    if((up_ + right_).y > 0) {
        // pos.y = resolution_.y - pos.y;
    }
    pos += pixel_projection_offset_;
    // std::cout << pixel_projection_offset_.x << ", " << pixel_projection_offset_.y << std::endl;
    return to_sf(pos);
} 

sf::Vector2f Drawable_Object::to_projection_size(glm::vec2 size) {
    if((up_ + right_).y < 0) {
        size = right_ * size.x + up_ * size.y;
    }
    else {
        size = right_ * size.x + up_ * size.y;
    }
    return sf::Vector2f{size.x * pixel_per_projection_, size.y * pixel_per_projection_};
}

glm::vec2 Drawable_Object::pixel_projection_offset() {
    return pixel_projection_offset_;
}
glm::vec2 Drawable_Object::pixel_projection_size() {
    return pixel_projection_size_;
}

void Drawable_Object::set_phys_table_size(glm::vec2 const& size) {
    physical_table_size_ = size;
}

sf::Vector2f to_sf(glm::vec2 const& vec) {
    return sf::Vector2f{vec.x, vec.y};
}

glm::vec2 to_glm(sf::Vector2f const& vec) {
    return glm::vec2{vec.x, vec.y};
}


};