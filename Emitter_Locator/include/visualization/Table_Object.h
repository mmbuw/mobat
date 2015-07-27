#ifndef TTT_TABLE_OBJECT_H
#define TTT_TABLE_OBJECT_H

#include "drawable_object.h"
#include "Microphone_Object.h"
#include <glm/glm.hpp>

namespace TTT {

class Table_Object : DrawableObject {

	private:
		sf::RectangleShape table_rectangle_shape_;
		sf::RectangleShape middle_line1_;
		sf::RectangleShape middle_line2_;
		sf::RectangleShape left_line_;
		sf::RectangleShape right_line_;
		sf::CircleShape middle_;
		sf::CircleShape left_goal_;
		sf::CircleShape right_goal_;

	public:
		virtual void Draw(sf::RenderWindow& canvas) const;
		void Draw(sf::RenderWindow& canvas, std::vector<Microphone_Object> const& microphones, glm::vec4 const& toas) const;
		virtual void Recalculate_Geometry();
		void Set_Fill_Color(sf::Color const& in_fill_color);
};


};

#endif