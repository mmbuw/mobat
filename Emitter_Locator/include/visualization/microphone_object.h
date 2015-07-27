#ifndef TTT_MicrophoneObject_H
#define TTT_MicrophoneObject_H

#include "drawable_object.h"

namespace TTT {

class MicrophoneObject : DrawableObject {

	private:
		unsigned id_;
		sf::CircleShape microphone_circle_shape_;

	public:
		glm::vec2 physical_position_;
		
		MicrophoneObject(unsigned in_id, glm::vec2 const& in_position );
		virtual void Draw(sf::RenderWindow& canvas) const;
		virtual void recalculateGeometry();

		void setFillColor(sf::Color const& in_fill_color);

};


};
#endif