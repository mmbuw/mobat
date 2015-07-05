#ifndef TTT_DRAWABLE_OBJECT_H
#define TTT_DRAWABLE_OBJECT_H

#include <SFML/Graphics.hpp>
#include <glm/vec2.hpp>

namespace TTT{

class Drawable_Object {
	public:
		Drawable_Object();
		virtual ~Drawable_Object();

		virtual void Draw(sf::RenderWindow& canvas) const = 0;
		//virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;
		static glm::vec2 const& get_phys_table_size();
		static void set_phys_table_size(glm::vec2 const& size);
		static void set_projection(glm::vec2 const& origin, glm::vec2 const& size);
		static void set_resolution(glm::vec2 const& res);

		static sf::Vector2f to_projection_space(glm::vec2 pos, float radius = 0.0f);
		static sf::Vector2f to_projection_size(glm::vec2 size);
		static glm::vec2 pixel_projection_offset();
		static glm::vec2 pixel_projection_size();

	// protected:

		//scaling factor for the elements attributes
		static glm::vec2 physical_table_size_;
		static glm::vec2 resolution_;
		static glm::vec2 right_;
		static glm::vec2 up_;
		static glm::vec2 physical_projection_offset_;
		static glm::vec2 pixel_projection_offset_;
		static glm::vec2 physical_projection_size_;
		static glm::vec2 pixel_projection_size_;
		static float pixel_per_projection_;
};

sf::Vector2f to_sf(glm::vec2 const& vec);
glm::vec2 to_glm(sf::Vector2f const& vec);

}; //namespace TTT
#endif