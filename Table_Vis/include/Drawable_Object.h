#ifndef TTT_DRAWABLE_OBJECT_H
#define TTT_DRAWABLE_OBJECT_H

#include <SFML/Graphics.hpp>

#include <limits>

namespace TTT{

class Drawable_Object {
	public:
		Drawable_Object() {};
		virtual ~Drawable_Object() {};

		virtual void Draw(sf::RenderWindow& canvas) const = 0;
		//virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

	protected:

		//scaling factor for the elements attributes
		static float pixel_per_meter_;
		static sf::Vector2f table_dims_in_px_;
		static sf::Vector2f physical_table_size_;
		static sf::Vector2u resolution_;

};

}; //namespace TTT
#endif