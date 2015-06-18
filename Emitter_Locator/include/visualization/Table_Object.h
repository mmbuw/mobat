#ifndef TTT_TABLE_OBJECT_H
#define TTT_TABLE_OBJECT_H

#include "Drawable_Object.h"
#include "Microphone_Object.h"
#include <glm/glm.hpp>

namespace TTT {

class Table_Object : Drawable_Object {

	private:
		sf::RectangleShape table_rectangle_shape_;

	public:
		virtual void Draw(sf::RenderWindow& canvas) const;
		void Draw(sf::RenderWindow& canvas, std::vector<Microphone_Object> const& microphones, glm::vec4 const& toas) const;
		virtual void Recalculate_Geometry();
		void Set_Fill_Color(sf::Color const& in_fill_color);

		void Set_Physical_Size(sf::Vector2f const& in_dimensions);
		sf::Vector2f Physical_Size() const;

};


};

#endif