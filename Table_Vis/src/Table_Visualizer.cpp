#include "../include/Table_Visualizer.h"

namespace TTT {

Table_Visualizer::
Table_Visualizer( sf::Vector2f const& Table_Dims,
				  std::vector<sf::Vector2f> const& microphone_positions ) {

}

Table_Visualizer::
~Table_Visualizer() {

}

void Table_Visualizer::
Draw(sf::RenderWindow& canvas) const {
	table_.Draw(canvas);

	for(auto const& mic_obj : microphones_) {
		mic_obj.Draw(canvas);
	}
}

void Table_Visualizer::
Resize_Physical_Table(sf::Vector2f const& Table_Dims ) {
	table_.Set_Physical_Size(Table_Dims);
}

void Table_Visualizer::
Reset_Microphones(std::vector<Microphone_Object> const& microphones) {
	microphones_ = microphones;
}

void Table_Visualizer::
Recalculate_Component_Geometry( sf::Vector2f const& resolution) {
	table_.Recalculate_Table_Geometry(resolution);
}

} //namespace TTT