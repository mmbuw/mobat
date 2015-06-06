#ifndef TTT_TABLE_OBJECT_H
#define TTT_TABLE_OBJECT_H

#include "Structs.h"
#include "Drawable_Object.h"

namespace TTT {

class Table_Object : Drawable_Object {

	private:
		sf::Vector2f physical_size_;
		sf::RectangleShape table_rectangle_shape_;

	public:
		virtual void Draw(sf::RenderWindow& canvas) const;

		void Set_Physical_Size(sf::Vector2f const& in_dimensions);
		void Recalculate_Table_Geometry(sf::Vector2f const& resolution);

		sf::Vector2f Physical_Table_Size() const;

};


};

#endif