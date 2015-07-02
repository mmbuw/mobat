#ifndef TTT_MICROPHONE_OBJECT_H
#define TTT_MICROPHONE_OBJECT_H

#include "Drawable_Object.h"

namespace TTT {

class Microphone_Object : Drawable_Object {

	private:
		unsigned id_;
		sf::CircleShape microphone_circle_shape_;

	public:
		glm::vec2 physical_position_;
		
		Microphone_Object(unsigned in_id, glm::vec2 const& in_position );
		virtual void Draw(sf::RenderWindow& canvas) const;
		virtual void Recalculate_Geometry();

		void Set_Fill_Color(sf::Color const& in_fill_color);

};


};
#endif