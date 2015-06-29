#include "Table_Visualizer.h"

#include <iostream>


namespace TTT {

float Drawable_Object::pixel_per_meter_ = 0.0f;
glm::vec2 Drawable_Object::table_dims_in_px_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::physical_table_size_ = glm::vec2(0.0f, 0.0f);
glm::vec2 Drawable_Object::resolution_ = glm::vec2(0,0);

Table_Visualizer::
Table_Visualizer( glm::vec2 const& in_canvas_resolution,
				  glm::vec2 const& table_dims,
				  std::vector<sf::Vector2f> const& microphone_positions,
				  sf::Color const& in_table_fill_color,
				  sf::Color const& in_microphone_fill_color,
				  sf::Color const& in_token_fill_color,
				  double ball_size) : elapsed_milliseconds_since_last_frame_(0) {

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
	//Set_Token_Fill_Color( in_token_fill_color );


	//pseudopong


	b_x_pos_ = physical_table_size_.x/2;
	b_y_pos_ = physical_table_size_.y/2;
	//b_x_pos_ = 1;
	//b_y_pos_ = 2;

	//ball_ = Ball(sf::Vector2f(b_x_pos_, b_y_pos_));
	ball_ = Ball(sf::Vector2f(b_x_pos_, b_y_pos_), ball_size);
	ball_dir_ = sf::Vector2f(0, 0.01);
	ball_.Set_Fill_Color(sf::Color::Red);
	ball_speed_ = 0.01;
	right_goals_ = 0;
	left_goals_ = 0;


	//get initial timestamp
	last_time_stamp_ = std::chrono::high_resolution_clock::now();
	//elapsed_milliseconds_since_last_frame_ = frame_timer_.getElapsedTime();
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
Resize_Physical_Table(glm::vec2 const& Table_Dims ) {
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



		for(auto const& i : recognized_tokens_){

				auto token = i.second.get_Circle();

				if(circ_circ_intersect(t_ball, token).first){
			    std::cout<<"Treffer!\n";

					ball_.should_move_ = false;

					    glm::vec2 normal;

					    move_ball_out_of_token(token);
					    normal = circ_circ_intersect(t_ball, token).second;
					    
					    
					    glm::vec2 reflect = glm::reflect( glm::vec2{ball_dir_.x, ball_dir_.y}, normal);

					    ball_dir_.x = reflect.x;
					    ball_dir_.y = reflect.y;
				}			

		}


	

#if 1
//AUs bzw Torerkennung, muss man noch entscheiden, was was ist
	auto b_rad = ball_.getRadius() / pixel_per_meter_;


	if(b_x_pos_ <= 0 + b_rad|| b_x_pos_>=physical_table_size_.x - b_rad){
		ball_dir_.x *= -1;
		if(b_x_pos_ <= 0 + b_rad){
			b_x_pos_ = 0 + b_rad;	
		}else if(b_x_pos_ >= physical_table_size_.x - b_rad){
			b_x_pos_ = physical_table_size_.x - b_rad;
		}
		//ball_.should_move_ = false;
	}


//TOOOOOOOR
	if(b_y_pos_ <= 0 - b_rad || b_y_pos_>=physical_table_size_.y +b_rad){
		if(b_y_pos_ <= 0- b_rad){
			ball_dir_.y = ball_speed_;
			++left_goals_;

		}else if(b_y_pos_ >= physical_table_size_.y + b_rad){
			ball_dir_.y = -ball_speed_;
			++right_goals_;
		}
		std::cout<<"Left: "<<left_goals_<<"  Right:  "<<right_goals_<<"\n";

		ball_dir_.x = 0;
			b_y_pos_ = physical_table_size_.y/2.0;
			b_x_pos_ = physical_table_size_.x/2.0;
		//ball_.should_move_ = false;
	}

#endif

		

/*	if(b_x_pos_ < resolution_.y && b_x_pos_ > -2*ball_.get_Circle().getRadius())
	{*/
//	}    
	
	

	   // b_x_pos_ += ball_dir_.x; //wieder rausnehmen

		//std::cout<<elapsed_milliseconds_since_last_frame_<<"\n";
	double factor = elapsed_milliseconds_since_last_frame_ * 1.0;

		b_x_pos_ += ball_dir_.x * factor;
		b_y_pos_ += ball_dir_.y * factor;


	
	ball_.should_move_ = true;

	ball_.setPosition(b_x_pos_, b_y_pos_);

	sf::CircleShape asdf;
	asdf.setPosition(b_x_pos_, b_y_pos_);

	ball_.give_new_Circle(asdf);


	ball_.Recalculate_Geometry();
}

}

void Table_Visualizer::
Set_Canvas_Resolution(glm::vec2 const& in_resolution ) {
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
Set_Token_Fill_Color(unsigned frequency, sf::Color const& in_token_fill_color ) {

	token_color_mapping_[frequency] = in_token_fill_color;

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
		tokens_to_refresh_.insert(in_id);
	}
		recognized_tokens_[in_id] = Recognized_Token_Object(in_id, in_position); 

		std::map<unsigned, sf::Color>::iterator token_color_mapping_iterator = 
			token_color_mapping_.find(in_id);
		if(token_color_mapping_.end() != token_color_mapping_.find(in_id) ) {
			recognized_tokens_[in_id]
				.Set_Fill_Color(token_color_mapping_iterator->second);
		}
	

}

void Table_Visualizer::
Finalize_Visualization_Frame() {
	Calculate_Elapsed_Milliseconds();
	unsigned int Elapsed_Milliseconds = Get_Elapsed_Milliseconds();
	
	std::set<unsigned> tokens_to_remove;

	for( auto& id_token_pair : recognized_tokens_ ) {

		bool refresh_token = false;
		if(tokens_to_refresh_.end() != 
		   tokens_to_refresh_.find(id_token_pair.first) ) {
			refresh_token = true;
		}


		
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


void Table_Visualizer::
Calculate_Elapsed_Milliseconds() {
	std::chrono::high_resolution_clock::time_point 
	current_time_stamp = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds elapsed_milliseconds
		= std::chrono::duration_cast<std::chrono::milliseconds>
		(current_time_stamp - last_time_stamp_); 

	last_time_stamp_ = current_time_stamp;

	//std::cout << "Elapsed milliseconds: " << elapsed_milliseconds.count()<<"\n";
	elapsed_milliseconds_since_last_frame_ = elapsed_milliseconds.count();
	//std::cout << "After: " << elapsed_milliseconds_since_last_frame_<<"\n";*/

	//elapsed_milliseconds_since_last_frame_ = frame_timer_.getElapsedTime();
	//frame_timer_.restart();
}

unsigned Table_Visualizer::
Get_Elapsed_Milliseconds(){
	return elapsed_milliseconds_since_last_frame_;//.asMilliseconds();
}

std::pair<bool, glm::vec2> Table_Visualizer::circ_circ_intersect(sf::CircleShape const& ball, sf::CircleShape const& paddle) const{
    glm::vec2 mid_ball{ball.getPosition().x + ball.getRadius(), ball.getPosition().y + ball.getRadius()};
    glm::vec2 mid_paddle{paddle.getPosition().x + paddle.getRadius(), paddle.getPosition().y + paddle.getRadius()};

    double dist = glm::length(mid_ball - mid_paddle);

    bool intersects = (glm::abs(dist) < (ball.getRadius() + paddle.getRadius()));

    //its in global_sys, hence the y coordinates have to be swapped for calulating the normal :(
    mid_ball.y += mid_paddle.y;
    mid_paddle.y = mid_ball.y - mid_paddle.y;
    mid_ball.y = mid_ball.y - mid_paddle.y;

    glm::vec2 normal = glm::normalize(mid_ball - mid_paddle);

    return std::pair<bool, glm::vec2>{intersects, normal}; 
}

void Table_Visualizer::move_ball_out_of_token(sf::CircleShape const& paddle){
	  
    glm::vec2 mid_ball{ball_.getPosition().x + ball_.getRadius(), ball_.getPosition().y + ball_.getRadius()};
    glm::vec2 mid_paddle{paddle.getPosition().x + paddle.getRadius(), paddle.getPosition().y + paddle.getRadius()};

    mid_ball *= physical_table_size_ / resolution_;
    mid_paddle *= physical_table_size_ / resolution_;
    
    auto norm = circ_circ_intersect(ball_.get_Circle(), paddle).second;
    
    float dist = glm::length(mid_ball - mid_paddle);
    
    auto tmp = mid_paddle * norm * dist;
    //tmp -= glm::vec2{ball_.getRadius(), ball_.getRadius()};

    glm::vec2 new_ball_pos{mid_paddle + norm * dist};
    new_ball_pos -= glm::vec2{ball_.getRadius(), ball_.getRadius()};

    b_x_pos_ += tmp.x;
    b_y_pos_ += tmp.y/2.0;
    // b_x_pos_ = new_ball_pos.x;
    // b_y_pos_ = new_ball_pos.y;
    // ball_.setPosition()
    ball_.setPosition(b_x_pos_, b_y_pos_);
 
    std::cout<<"paddle: "<<mid_paddle.x << "  "<<mid_paddle.y<<"\n";
    std::cout<<"ball: "<<mid_ball.x << "  "<<mid_ball.y<<"\n";
    std::cout<<"norm: "<<norm.x << "  "<<norm.y<<"\n";
    std::cout<<"dist: "<<dist<<"\n";
    std::cout<<"newpos: "<<tmp.x << "  "<<tmp.y<<"\n";
}


}; //namespace TTT
