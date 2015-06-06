#ifndef TTT_MICROPHONE_OBJECT_H
#define TTT_MICROPHONE_OBJECT_H

#include "Structs.h"

#include "Drawable_Object.h"



namespace TTT {

class Microphone_Object : Drawable_Object {

	private:
		sf::Vector2f pos_;
		sf::CircleShape microphone_circle_shape_;

	public:
		virtual void Draw(sf::RenderWindow& canvas) const;

		void Set_Position(sf::Vector2f const& in_pos);
		sf::Vector2f Position() const;


};


};
#endif