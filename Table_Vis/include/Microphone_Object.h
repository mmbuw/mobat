#ifndef TTT_MICROPHONE_OBJECT_H
#define TTT_MICROPHONE_OBJECT_H

#include "Drawable_Object.h"



namespace TTT {

class Microphone_Object : Drawable_Object {

	private:
		unsigned id_;
		sf::Vector2f physical_position_;
		sf::CircleShape microphone_circle_shape_;

	public:
		Microphone_Object(unsigned in_id, sf::Vector2f in_position );
		virtual void Draw(sf::RenderWindow& canvas) const;
		virtual void Recalculate_Geometry(sf::Vector2f const& resolution,
										  sf::Vector2f const& physical_table_size);

};


};
#endif