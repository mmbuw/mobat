#include "Table_Visualizer.h"

#include <iostream>


namespace TTT {

float Drawable_Object::pixel_per_meter_ = 0.0f;
sf::Vector2f Drawable_Object::table_dims_in_px_ = sf::Vector2f(0.0f, 0.0f);
sf::Vector2f Drawable_Object::physical_table_size_ = sf::Vector2f(0.0f, 0.0f);
sf::Vector2u Drawable_Object::resolution_ = sf::Vector2u(0,0);

Table_Visualizer::
Table_Visualizer( sf::Vector2u const& in_canvas_resolution,
				  sf::Vector2f const& table_dims,
				  std::vector<sf::Vector2f> const& microphone_positions,
				  sf::Color const& in_table_fill_color,
				  sf::Color const& in_microphone_fill_color,
				  sf::Color const& in_token_fill_color)
				  {

	resolution_ = in_canvas_resolution;

	unsigned int id_counter = 0;

	if(!microphone_positions.empty()) {
		for(auto const& pos : microphone_positions ) {
			microphones_.push_back(Microphone_Object(id_counter++, pos) );
		}
	} else {
		std::vector<sf::Vector2f> default_microphone_positions_;
			default_microphone_positions_.push_back(sf::Vector2f(0.0,0.0));
			default_microphone_positions_.push_back(sf::Vector2f(0.0, table_dims.y));
			default_microphone_positions_.push_back(sf::Vector2f(table_dims.x,0.0));
			default_microphone_positions_.push_back(sf::Vector2f(table_dims.x, table_dims.y));

		for(auto const& pos : default_microphone_positions_) {
		microphones_.push_back(Microphone_Object(id_counter++, pos) );
		}
	}
	
	Resize_Physical_Table( table_dims ); 
	Set_Table_Fill_Color( in_table_fill_color );
	Set_Microphone_Fill_Color( in_microphone_fill_color );
	Set_Token_Fill_Color( in_token_fill_color );

	//get initial timestamp
	last_time_stamp_ = std::chrono::high_resolution_clock::now();
}

Table_Visualizer::
~Table_Visualizer() {}

void Table_Visualizer::
Draw(sf::RenderWindow& canvas) const {
	table_.Draw(canvas);

	for(auto const& mic_obj : microphones_) {
		mic_obj.Draw(canvas);
	}

	for( auto& id_token_pair : recognized_tokens_ ) {
		id_token_pair.second.Draw(canvas);
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
Recalculate_Geometry() {

	table_.Recalculate_Geometry();

	for(auto& mic_obj : microphones_) {
		mic_obj.Recalculate_Geometry();
	}

	for( auto& id_token_pair : recognized_tokens_ ) {
		id_token_pair.second.Recalculate_Geometry();
	}
}

void Table_Visualizer::
Set_Canvas_Resolution( sf::Vector2u const& in_resolution ) {
	resolution_ = in_resolution;
}

void Table_Visualizer::
Set_Table_Fill_Color( sf::Color const& in_table_fill_color ) {
	table_.Set_Fill_Color(in_table_fill_color);
}

void Table_Visualizer::
Set_Microphone_Fill_Color( sf::Color const& in_microphone_fill_color ) {

	for(auto& mic : microphones_) {
		mic.Set_Fill_Color(in_microphone_fill_color);
	}

}

void Table_Visualizer::
Set_Token_Fill_Color( sf::Color const& in_token_fill_color ) {
	for( auto& id_token_pair : recognized_tokens_ ) {
		id_token_pair.second.Set_Fill_Color(in_token_fill_color);
	}
}

void Table_Visualizer::
Set_Token_Recognition_Timeout( unsigned in_timeout_in_ms ) {
	for( auto& id_token_pair : recognized_tokens_ ) {
		id_token_pair.second.Set_Life_Time(in_timeout_in_ms);
	}
}

void Table_Visualizer::
Signal_Token(unsigned int in_id, sf::Vector2f const& in_position) {

	if( recognized_tokens_.end() != recognized_tokens_.find(in_id) ) {
		//std::cout << "Insert token id: " << in_id << "\n";
		tokens_to_refresh_.insert(in_id);
	}
		//std::cout << "Inserting new token with id: " << in_id << "\n";
		recognized_tokens_[in_id] = Recognized_Token_Object(in_id, in_position); 
	

}

void Table_Visualizer::
Finalize_Visualization_Frame() {
	std::set<unsigned> tokens_to_remove;

	for( auto& id_token_pair : recognized_tokens_ ) {
		//std::cout << "lt: " << id_token_pair.second.remaining_life_time_in_ms_ << "\n";
		bool refresh_token = false;
		if(tokens_to_refresh_.end() != 
		   tokens_to_refresh_.find(id_token_pair.first) ) {
			refresh_token = true;
		}

		//std::cout << "Before Update Token: " << refresh_token << "\n";
		//token signaled lifetime zero -> register to remove it

		unsigned int Elapsed_Milliseconds = Get_Elapsed_Milliseconds();

		//std::cout << "Elapsed Milliseconds: " << Elapsed_Milliseconds << "\n";
		
		if( ! id_token_pair.second
			.Update_Token(refresh_token, 
						  Elapsed_Milliseconds, 
						  id_token_pair.second.physical_position_) ) {
			tokens_to_remove.insert(id_token_pair.first);
		}

	}

	for(auto const& token_id : tokens_to_remove ) {
		recognized_tokens_.erase(token_id);
	}

	tokens_to_refresh_.clear();
}


unsigned Table_Visualizer::
Get_Elapsed_Milliseconds(){
	std::chrono::high_resolution_clock::time_point 
		current_time_stamp = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds elapsed_milliseconds 
		= std::chrono::duration_cast<std::chrono::milliseconds>
		(current_time_stamp - last_time_stamp_); 

	last_time_stamp_ = current_time_stamp;

	return elapsed_milliseconds.count();
}

}; //namespace TTT