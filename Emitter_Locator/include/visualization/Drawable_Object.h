#ifndef TTT_DRAWABLE_OBJECT_H
#define TTT_DRAWABLE_OBJECT_H

#include <SFML/Graphics.hpp>
#include <glm/vec2.hpp>
#include <limits>

namespace TTT{

class Drawable_Object {
	public:
		Drawable_Object() {};
		virtual ~Drawable_Object() {};

		virtual void Draw(sf::RenderWindow& canvas) const = 0;
		//virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;
		static glm::vec2 const& get_phys_table_size() { return physical_table_size_;};
		static void set_phys_table_size(glm::vec2 const& size) {physical_table_size_ = size;};

		static void recalculate_measures() {
			//ensure 5% of border at each side
			sf::Vector2f res_with_margin = //resolution;
						sf::Vector2f(resolution_.x - 0.1*resolution_.x,
									 resolution_.y - 0.1*resolution_.y);

			pixel_per_meter_ = std::min(res_with_margin.x / physical_table_size_.x, 
										res_with_margin.y / physical_table_size_.y);

			table_dims_in_px_  = physical_table_size_ * pixel_per_meter_;

			pixel_table_offset_ = glm::vec2{(resolution_.x - table_dims_in_px_.x)/2.0,
									    (resolution_.y - table_dims_in_px_.y)/2.0};
			right_ = glm::vec2{1.0f, 0.0f};
			up_ = glm::vec2{0.0f, 1.0f};


		}

		static sf::Vector2f to_pixel_space(glm::vec2 const& pos, float radius) {
			glm::vec2 pixel_pos{
			 right_ * (pixel_table_offset_.x + pos.x * pixel_per_meter_  - radius)
			 + up_ * (pixel_table_offset_.y + pos.y * pixel_per_meter_ - radius)};
			return sf::Vector2f{pixel_pos.x, pixel_pos.y};
		}

	protected:

		//scaling factor for the elements attributes
		static float pixel_per_meter_;
		static glm::vec2 physical_table_size_;
		static glm::vec2 pixel_table_offset_;
		static glm::vec2 table_dims_in_px_;
		static glm::vec2 resolution_;
		static glm::vec2 right_;
		static glm::vec2 up_;

};

}; //namespace TTT
#endif