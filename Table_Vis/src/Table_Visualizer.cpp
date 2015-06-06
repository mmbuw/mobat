#include "../include/Table_Visualizer.h"

namespace TTT {

Table_Visualizer::
Table_Visualizer() {

	std::vector<sf::Vector2f> default_microphone_positions_;
		default_microphone_positions_.push_back(sf::Vector2f(0.0,0.0));
		default_microphone_positions_.push_back(sf::Vector2f(0.0,1.0));
		default_microphone_positions_.push_back(sf::Vector2f(2.0,0.0));
		default_microphone_positions_.push_back(sf::Vector2f(2.0,1.0));

	unsigned int id_counter = 0;
	for(auto const& pos : default_microphone_positions_) {
		microphones_.push_back(Microphone_Object(id_counter++, pos) );
	}

	Resize_Physical_Table(sf::Vector2f(2.0f, 1.0f) ); 
}

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
Recalculate_Geometry( sf::Vector2f const& resolution) {
	table_.Recalculate_Geometry(resolution);

	for(auto& mic_obj : microphones_) {
		mic_obj.Recalculate_Geometry(resolution, 
									 table_.Physical_Size() );
	}
}

void Table_Visualizer::
Set_Table_Fill_Color( sf::Color const& in_table_fill_color ) {
	table_.Set_Fill_Color(in_table_fill_color);
}

} //namespace TTT