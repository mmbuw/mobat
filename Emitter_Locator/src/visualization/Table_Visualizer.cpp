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
}

Table_Visualizer::
~Table_Visualizer() {}


void Table_Visualizer::
Draw(sf::RenderWindow& canvas) const {
	table_.Draw(canvas);

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
    std::cout<<"Treffer!\n";

		ball_.should_move_ = false;

		    glm::vec2 normal;

		    ball_.Set_Fill_Color(sf::Color::Red);
		    if(circ_circ_intersect(t_ball, right).first)
		    {
		        //rechter spieler schaffts
		        move_ball_out_of_token(right);
		        normal = circ_circ_intersect(t_ball, right).second;
		        //ball_dir_.x = 0;
		    }
		    else
		    {
		        //linker spieler schaffts
		        move_ball_out_of_token(left);
		        normal = circ_circ_intersect(t_ball, left).second;
		        //ball_dir_.x = 0;
		    }
		    glm::vec2 reflect = glm::reflect( glm::vec2{ball_dir_.x, ball_dir_.y}, normal);

		    ball_dir_.x = reflect.x;
		    ball_dir_.y = reflect.y;
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

	

		b_x_pos_ += ball_dir_.x;
		b_y_pos_ += ball_dir_.y;


	
	ball_.should_move_ = true;

	ball_.setPosition(b_x_pos_, b_y_pos_);

	sf::CircleShape asdf;
	asdf.setPosition(b_x_pos_, b_y_pos_);

	ball_.give_new_Circle(asdf);


	ball_.Recalculate_Geometry();
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

std::pair<bool, glm::vec2> Table_Visualizer::circ_circ_intersect(sf::CircleShape const& ball, sf::CircleShape const& paddle) const{
    double x1 = ball.getPosition().x + ball.getRadius(), y1 = ball.getPosition().y /*+ ball.getRadius()*/;
    double x2 = paddle.getPosition().x + paddle.getRadius(), y2 = paddle.getPosition().y /*+ paddle.getRadius()*/;



    glm::vec2 mid_ball{x1,y1};
    glm::vec2 mid_paddle{x2,y2};
    double dist = glm::length(mid_ball - mid_paddle);
    //double dist = glm::length(mid_paddle - mid_ball);



    

    bool tmp1 = (dist < (ball.getRadius() + paddle.getRadius() ) && dist > abs(ball.getRadius() - paddle.getRadius()));
    

    //its in global_sys, hence the y coordinates have to be swapped for claculating the normal :(
    mid_ball.y += mid_paddle.y;
    mid_paddle.y = mid_ball.y - mid_paddle.y;
    mid_ball.y = mid_ball.y - mid_paddle.y;

    glm::vec2 normal = glm::normalize(mid_ball - mid_paddle);


    return std::pair<bool, glm::vec2>{tmp1, normal}; 

}

void Table_Visualizer::move_ball_out_of_token(sf::CircleShape const& t){
	 
	double x1 = t.getPosition().x + t.getRadius(), y1 = t.getPosition().y + t.getRadius();
    double x2 = ball_.getPosition().x + ball_.getRadius(), y2 = ball_.getPosition().y + ball_.getRadius();
    


    x1 *= (physical_table_size_.x / resolution_.x);
    y1 *= (physical_table_size_.y / resolution_.y);
    x2 *= (physical_table_size_.x / resolution_.x);
    y2 *= (physical_table_size_.y / resolution_.y);


    glm::vec2 mid_paddle{x1,y1};
    glm::vec2 mid_ball{x2,y2};

    
    std::cout<<"paddle: "<<mid_paddle.x << "  "<<mid_paddle.y<<"\n";
    std::cout<<"ball: "<<mid_ball.x << "  "<<mid_ball.y<<"\n";

    auto norm = circ_circ_intersect(ball_.get_Circle(), t).second;

    std::cout<<"norm: "<<norm.x << "  "<<norm.y<<"\n";
    

    auto dist = glm::length(mid_ball - mid_paddle)  ;
    
    std::cout<<"dist: "<<dist<<"\n";

    


    auto tmp = mid_paddle * norm * float(dist);
    //tmp -= glm::vec2{ball_.getRadius(), ball_.getRadius()};

    std::cout<<"newpos: "<<tmp.x << "  "<<tmp.y<<"\n";
    

    
    b_x_pos_ += tmp.x;
    b_y_pos_ += tmp.y/2.0;

    ball_.setPosition(b_x_pos_, b_y_pos_);

}


}; //namespace TTT