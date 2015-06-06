#ifndef TTT_DRAWABLE_OBJECT_H
#define TTT_DRAWABLE_OBJECT_H

#include <SFML/Graphics.hpp>

#include <limits>


class Drawable_Object {
	public:
		Drawable_Object() {};
		virtual ~Drawable_Object() {};

		virtual void Draw(sf::RenderWindow& canvas) const = 0;
		//virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

};

#endif