#include "Drawable_Object.h"

namespace TTT {

glm::vec2 Drawable_Object::physical_table_size_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::resolution_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::up_ = glm::vec2(1,0);
glm::vec2 Drawable_Object::right_ = glm::vec2(0,1);
glm::vec2 Drawable_Object::pixel_projection_offset_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::physical_projection_offset_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::physical_projection_size_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::pixel_projection_size_ = glm::vec2(0,0);
float Drawable_Object::pixel_per_projection_ = 0.0f;

Drawable_Object::Drawable_Object()
{};

Drawable_Object::~Drawable_Object()
{};

void Drawable_Object::set_projection(glm::vec2 const& origin, glm::vec2 const& size) {
    physical_projection_offset_ = origin;
    physical_projection_size_ = size;

    if(size.x > size.y) {
        pixel_per_projection_ = resolution_.x / size.x;
        pixel_projection_offset_ = {0, (resolution_.y - size.y * pixel_per_projection_ ) * 0.5f};
    }
    else {
        pixel_per_projection_ = resolution_.y / size.y;
        pixel_projection_offset_ = {(resolution_.x - size.x * pixel_per_projection_ ) * 0.5f, 0};
    }

    pixel_projection_size_ = physical_projection_size_ * pixel_per_projection_;
}

void Drawable_Object::set_resolution(glm::vec2 const& res) {
    resolution_ = res;   
}

sf::Vector2f Drawable_Object::to_projection_space(glm::vec2 pos, float radius) {
    pos -= physical_projection_offset_;
    pos *= pixel_per_projection_;
    pos -= glm::vec2{radius, radius};
    // add margin
    pos += pixel_projection_offset_;
    pos.y = resolution_.y - pos.y;
    return to_sf(pos);
} 

sf::Vector2f Drawable_Object::to_projection_size(glm::vec2 const& size) {
    return sf::Vector2f{size.x * pixel_per_projection_, size.y * -pixel_per_projection_};
}


sf::Vector2f to_sf(glm::vec2 const& vec) {
    return sf::Vector2f{vec.x, vec.y};
}

glm::vec2 to_glm(sf::Vector2f const& vec) {
    return glm::vec2{vec.x, vec.y};
}

};