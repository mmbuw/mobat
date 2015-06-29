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
	protected:

		//scaling factor for the elements attributes
		static float pixel_per_meter_;
		static glm::vec2 physical_table_size_;
		static glm::vec2 table_dims_in_px_;
		static glm::vec2 resolution_;

};

}; //namespace TTT
#endif