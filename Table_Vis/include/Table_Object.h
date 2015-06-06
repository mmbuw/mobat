#ifndef TTT_TABLE_OBJECT_H
#define TTT_TABLE_OBJECT_H

#include "Drawable_Object.h"

namespace TTT {

class Table_Object : Drawable_Object {

	private:
		sf::Vector2f physical_size_;
		sf::RectangleShape table_rectangle_shape_;

	public:
		virtual void Draw(sf::RenderWindow& canvas) const;
		virtual void Recalculate_Geometry(sf::Vector2f const& resolution);
		void Set_Fill_Color(sf::Color const& in_fill_color);

		void Set_Physical_Size(sf::Vector2f const& in_dimensions);
		sf::Vector2f Physical_Size() const;

};


};

#endif