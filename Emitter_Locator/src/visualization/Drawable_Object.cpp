#include "Drawable_Object.h"

namespace TTT {

float Drawable_Object::pixel_per_meter_ = 0.0f;
glm::vec2 Drawable_Object::pixel_table_size_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::physical_table_size_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::pixel_table_offset_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::resolution_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::up_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::right_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::projection_offset_ = glm::vec2(0,0);
glm::vec2 Drawable_Object::projection_size_ = glm::vec2(0,0);

Drawable_Object::Drawable_Object()
{};

Drawable_Object::~Drawable_Object()
{};

glm::vec2 const& Drawable_Object::get_phys_table_size() {
 return physical_table_size_;
}

void Drawable_Object::set_phys_table_size(glm::vec2 const& size) {
    physical_table_size_ = size;
};

void Drawable_Object::set_projection(glm::vec2 const& origin, glm::vec2 const& size) {
    projection_offset_ = origin;
    projection_size_ = size;
}

void Drawable_Object::set_resolution(glm::vec2 const& res) {
    resolution_ = res;   
}

void Drawable_Object::recalculate_measures() {
    //ensure 5% of border at each side
    sf::Vector2f res_with_margin = //resolution;
                sf::Vector2f(resolution_.x - 0.1*resolution_.x,
                             resolution_.y - 0.1*resolution_.y);

    pixel_per_meter_ = std::min(res_with_margin.x / physical_table_size_.x, 
                                res_with_margin.y / physical_table_size_.y);

    pixel_table_size_  = physical_table_size_ * pixel_per_meter_;

    pixel_table_offset_ = glm::vec2{(resolution_.x - pixel_table_size_.x)/2.0,
                                (resolution_.y - pixel_table_size_.y)/2.0};
    right_ = glm::vec2{1.0f, 0.0f};
    up_ = glm::vec2{0.0f, 1.0f};
}

sf::Vector2f Drawable_Object::to_pixel_space(glm::vec2 const& pos, float radius) {
    glm::vec2 pixel_pos{
     right_ * (pixel_table_offset_.x + pos.x * pixel_per_meter_  - radius)
     + up_ * (resolution_.y - (pixel_table_offset_.y + pos.y * pixel_per_meter_) - radius)};
    return sf::Vector2f{pixel_pos.x, pixel_pos.y};
}

};