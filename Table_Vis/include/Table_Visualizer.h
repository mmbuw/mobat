#ifndef TABLE_VISUALIZER_H
#define TABLE_VISUALIZER_H

#include "Table_Object.h"
#include "Microphone_Object.h"

#include <vector>

namespace TTT {




class Table_Visualizer : Drawable_Object {

	private:
		Table_Object table_;
		std::vector<Microphone_Object> microphones_;
		std::vector<Frequency_Position_Pair> recognized_positions_;

	public:

		Table_Visualizer(sf::Vector2f const& table_dims, 
						 std::vector<sf::Vector2f> const& microphone_positions );
		~Table_Visualizer();

		virtual void Draw(sf::RenderWindow& canvas) const;

		void Resize_Physical_Table(sf::Vector2f const& table_dims);

		void Reset_Microphones(std::vector<Microphone_Object> const& microphones);

		void Recalculate_Component_Geometry(sf::Vector2f const& resolution_);



};

};
#endif