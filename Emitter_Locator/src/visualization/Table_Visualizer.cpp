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
				  sf::Color const& in_token_fill_color,
				  double ball_size){

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


	//pseudopong

	ball_pos_.x = 1;
	ball_pos_.y = 0.5;
	ball_ = Recognized_Token_Object(0, sf::Vector2f(ball_pos_.x, ball_pos_.y));
	ball_dir_ = glm::vec2(-0.01, 0);
	ball_.Set_Fill_Color(sf::Color::Red);
	ball_speed_ = 0.01;


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

	ball_.Draw(canvas);
	
}


void Table_Visualizer::
Draw(sf::RenderWindow& canvas, glm::vec4 toas) const {
	
	table_.Draw(canvas, microphones_, toas );

	for(auto const& mic_obj : microphones_) {
		mic_obj.Draw(canvas);
	}

	for( auto& id_token_pair : recognized_tokens_ ) {
		id_token_pair.second.Draw(canvas);
	}

	ball_.Draw(canvas);
	

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



// pseudopong zeug
	//ballschlag
//std::cout<<recognized_tokens_.size()<<"\n";
/*for( auto& id_token_pair : recognized_tokens_ ) {
		std::cout<<id_token_pair.first<<"\n";
	}
*/

	if(recognized_tokens_.size() >= 2){

		sf::CircleShape left  = recognized_tokens_[16000].get_Circle();	//NO HARDCODED FREQUENCIES!!!!!!!!!!!!
		sf::CircleShape right = recognized_tokens_[18000].get_Circle();
		sf::CircleShape t_ball = ball_.get_Circle();


		if(circ_circ_intersect(t_ball, right).first || circ_circ_intersect(t_ball, left).first)
		{

		    int rand_x = 0;
		    int rand_y = 0;

		    ball_.Set_Fill_Color(sf::Color::Red);
		    if(circ_circ_intersect(t_ball, right).first)
		    {
		        //rechter spieler schaffts
		        ball_dir_.x = -rand_x;
		        ball_dir_.y = rand_y;
		        //ball_dir_.x = 0;
		    }
		    else
		    {
		        //linker spieler schaffts
		        ball_dir_.x = rand_x;
		        ball_dir_.y = rand_y;
		        //ball_dir_.x = 0;

		    }
		}else{
		    ball_.Set_Fill_Color(sf::Color::White);
		}
	}

	//tor-erkennung
	if(ball_pos_.x < resolution_.x && ball_pos_.x > -2*ball_.get_Circle().getRadius())
	{
	    ball_pos_.x += ball_dir_.x;
	}    
	else
	{
	   // std::cout<<"TOOOOOORRRRR\n";
	    if(ball_pos_.x >= resolution_.x)
	    {
	        //tor für spieler left
	        ball_dir_.x = ball_speed_;
	        ball_dir_.y = 0;
	        //++left_goals; 
	    }
	    else
	    {
	        //tor für spieler right
	        ball_dir_.x = -ball_speed_;
	        ball_dir_.y = 0;
	        //++right_goals;
	    }
	    ball_pos_.x  = resolution_.x/2.0 - ball_.get_Circle().getRadius();
	    ball_pos_.y  = resolution_.y/2.0 - ball_.get_Circle().getRadius();
	    
	    /*l_y_pos  = y_max/2.0 - left.getRadius();
	    r_y_pos  = y_max/2.0 - right.getRadius();

	    l_x_pos  = 0;
	    r_x_pos  = resolution_.x - 2*right.getRadius();*/
	}



	// oben unten begrenzung
	if(ball_pos_.y >= resolution_.x - 2*ball_.get_Circle().getRadius() || ball_pos_.y <= 0)
	{
	    ball_dir_.y = -ball_dir_.y; //an obere kante gestoßen
	} 
	
	ball_pos_.y += ball_dir_.y;

	ball_ = Recognized_Token_Object(0, sf::Vector2f(ball_pos_.x, ball_pos_.y)); //when type of ball change, use sth like setPosition

	ball_.Recalculate_Geometry();


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

		unsigned int Elapsed_Milliseconds = Get_Elapsed_Milliseconds();
		
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

std::pair<bool, sf::Vector2f> Table_Visualizer::circ_circ_intersect(sf::CircleShape const& c1, sf::CircleShape const& c2) const{
    double x1 = c1.getPosition().x + c1.getRadius(), y1 = c1.getPosition().y + c1.getRadius();
    double x2 = c2.getPosition().x + c2.getRadius(), y2 = c2.getPosition().y + c2.getRadius();

    glm::vec2 mid_1{x1,y1};
    glm::vec2 mid_2{x2,y2};
    double dist = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));

    //std::cout<<c1.getPosition().x<<"  "<<c1.getPosition().y<<"  "<<c1.getRadius()<<"   "<<c2.getPosition().x<<"  "<<c2.getPosition().y<<"  "<<c2.getRadius()<<"\n";


    bool tmp1 = (dist < (c1.getRadius() + c2.getRadius() ) && dist > abs(c1.getRadius() - c2.getRadius()));
    sf::Vector2f normal{float(x1-x2), float(y1-y2)};


    return std::pair<bool, sf::Vector2f>{tmp1, normal}; 

}

}; //namespace TTT