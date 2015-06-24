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

	b_x_pos_ = 0.5;
	b_y_pos_ = 0.5;
	//ball_ = Ball(sf::Vector2f(b_x_pos_, b_y_pos_));
	ball_ = Ball(sf::Vector2f(b_x_pos_, b_y_pos_), ball_size);
	ball_dir_ = sf::Vector2f(-0.01, 0);
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


	
/*while(circ_circ_intersect(ball_.get_Circle(), right).first || circ_circ_intersect(ball_.get_Circle(), left).first){
		    b_x_pos_ -= ball_dir_.x;
		    b_y_pos_ -= ball_dir_.y;
		    ball_.setPosition(b_x_pos_, b_y_pos_);
}*/
		
#if 1
	//tor-erkennung
	if(b_x_pos_ < resolution_.y && b_x_pos_ > -2*ball_.get_Circle().getRadius())
	{
	    b_x_pos_ += ball_dir_.x;
	}    
	else
	{
	   // std::cout<<"TOOOOOORRRRR\n";
	    if(b_x_pos_ >= resolution_.y)
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
	    b_x_pos_  = resolution_.x/2.0 - ball_.get_Circle().getRadius();
	    b_y_pos_  = resolution_.y/2.0 - ball_.get_Circle().getRadius();
	    
	    /*l_y_pos  = y_max/2.0 - left.getRadius();
	    r_y_pos  = y_max/2.0 - right.getRadius();

	    l_x_pos  = 0;
	    r_x_pos  = resolution_.x - 2*right.getRadius();*/
	}



	// oben unten begrenzung
	if(b_y_pos_ >= resolution_.y - 2*ball_.get_Circle().getRadius() || b_y_pos_ <= 0)
	{
	    ball_dir_.y = -ball_dir_.y; //an obere kante gestoßen
	} 
	
	b_y_pos_ += ball_dir_.y;
#endif
	   // b_x_pos_ += ball_dir_.x; //wieder rausnehmen

	if(circ_circ_intersect(t_ball, right).first || circ_circ_intersect(t_ball, left).first)
		{
    std::cout<<"Treffer!\n";


		    glm::vec2 normal;

		    ball_.Set_Fill_Color(sf::Color::Red);
		    if(circ_circ_intersect(t_ball, right).first)
		    {
		        //rechter spieler schaffts
		        normal = circ_circ_intersect(t_ball, right).second;
		        move_ball_out_of_token(right);
		        //ball_dir_.x = 0;
		    }
		    else
		    {
		        //linker spieler schaffts
		        normal = circ_circ_intersect(t_ball, left).second;
		        move_ball_out_of_token(left);
		        //ball_dir_.x = 0;
		    }
		    glm::vec2 reflect = glm::reflect( glm::vec2{ball_dir_.x, ball_dir_.y}, normal);
std::cout<<"ball direction: "<< glm::normalize(glm::vec2{ball_dir_.x, ball_dir_.y}).x << "   " << glm::normalize(glm::vec2{ball_dir_.x , ball_dir_.y}).y <<"\n";
		    ball_dir_.x = reflect.x;
		    ball_dir_.y = reflect.y;


/*glm::vec2  in{0.5, -0.5};
glm::vec2 norm{0,1};
glm::vec2 out = glm::reflect(in, norm);*/
std::cout<<"normal: "<< glm::normalize(normal).x << "   " << glm::normalize(normal).y <<"\n";
std::cout<<"reflection: "<< glm::normalize(reflect).x << "   " << glm::normalize(reflect).y <<"\n";
		    /*ball_dir_.x = 0;
		    ball_dir_.y = 0;*/



		}/*else{
		    ball_.Set_Fill_Color(sf::Color::Red);
		}*/ 
	ball_.setPosition(b_x_pos_, b_y_pos_);


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
    double x1 = ball.getPosition().x + ball.getRadius(), y1 = ball.getPosition().y + ball.getRadius();
    double x2 = paddle.getPosition().x + paddle.getRadius(), y2 = paddle.getPosition().y + paddle.getRadius();

   /* x1 *= (physical_table_size_.x / resolution_.x);
    y1 *= (physical_table_size_.y / resolution_.y);
    x2 *= (physical_table_size_.x / resolution_.x);
    y2 *= (physical_table_size_.y / resolution_.y);*/


    glm::vec2 mid_ball{x1,y1};
    glm::vec2 mid_paddle{x2,y2};
    double dist = glm::length(mid_ball - mid_paddle);
    //double dist = glm::length(mid_paddle - mid_ball);

    //std::cout<<ball.getPosition().x<<"  "<<ball.getPosition().y<<"  "<<ball.getRadius()<<"   "<<paddle.getPosition().x<<"  "<<paddle.getPosition().y<<"  "<<paddle.getRadius()<<"\n";


    bool tmp1 = (dist < (ball.getRadius() + paddle.getRadius() ) && dist > abs(ball.getRadius() - paddle.getRadius()));
    

    //its in global_sys, hence the y coordinates have to be swapped for claculating the normal :(
    mid_ball.y += mid_paddle.y;
    mid_paddle.y = mid_ball.y - mid_paddle.y;
    mid_ball.y = mid_ball.y - mid_paddle.y;

    glm::vec2 normal = glm::normalize(mid_ball - mid_paddle);

if(tmp1){
    /*auto n = glm::normalize(glm::vec2{3, -4});
    std::cout<<"test:   " << n.x <<"  "<<n.y<<"\n";*/
    std::cout<<"Normal in intersect:   " << x1 << "-" <<x2<< " = " <<normal.x <<"  "<<y1 << "-" <<y2<< " = " <<normal.y<<"\n";
}
    return std::pair<bool, glm::vec2>{tmp1, normal}; 

}

void Table_Visualizer::move_ball_out_of_token(sf::CircleShape const& t){
	double x1 = t.getPosition().x + t.getRadius(), y1 = t.getPosition().y + t.getRadius();
    double x2 = ball_.getPosition().x + ball_.getRadius(), y2 = ball_.getPosition().y + ball_.getRadius();
    

    x1 *= (physical_table_size_.x / resolution_.x);
    y1 *= (physical_table_size_.y / resolution_.y);
    x2 *= (physical_table_size_.x / resolution_.x);
    y2 *= (physical_table_size_.y / resolution_.y);


    y1 = physical_table_size_.y -y1;
    y2 = physical_table_size_.y -y2;
    
/*    glm::vec2 mid_1{x1*physical_table_size_.x/resolution_.x,y1*physical_table_size_.y/resolution_.y};
    glm::vec2 mid_2{x2*physical_table_size_.x/resolution_.x,y2*physical_table_size_.y/resolution_.y};*/
    std::cout<<"starting to push out\n";

    glm::vec2 mid_paddle{x1,y1};
    glm::vec2 mid_ball{x2,y2};

    auto ball_rad = ball_.getRadius() / pixel_per_meter_; 
    auto t_rad = t.getRadius() / pixel_per_meter_; 

    auto norm = circ_circ_intersect(ball_.get_Circle(), t).second;
    std::cout<<norm.x<<"\n";

    //std::cout<<"y1:  "<<y1<<"   y2:  "<< y2<<"\n";

#if 1   //dumb idea, not allways working
    //x-koord is working (i think)
    //y-koord is NOT working
    //fuck that
    auto dist = glm::length(mid_ball - mid_paddle)  ;
    auto dist_x = dist -(t_rad + ball_rad) ;
    auto dist_y = dist -(t_rad + ball_rad) ;
    std::cout<<dist<<"  "<< pixel_per_meter_<<"\n";
    //dist *= 1/pixel_per_meter_; //transform distance to tabe_sys
    std::cout<<dist<<"  "<< pixel_per_meter_<<"\n";

//if(circ_circ_intersect(ball_.get_Circle(), t).first){

	std::cout<<"Distance:  "<<dist<<"\n";
//if(abs(dist) > 0.01){
	//if(abs(norm.x) > 0)
    	b_x_pos_ -= norm.x * dist_x ;// * physical_table_size_.x / resolution_.x;//transform 
    //if(abs(norm.y) > 0)
    	b_y_pos_ -= norm.y * dist_y ;// * physical_table_size_.y / resolution_.y;

    ball_.setPosition(b_x_pos_, b_y_pos_);

    std::cout<<"x_pos:  "<<b_x_pos_<<"   y_pos:  "<< b_y_pos_<<"\n";
//}
#else
    #if 1
    //jakobs try, maybe smart idea, never working


	//auto t_rad = t.getRadius()/pixel_per_meter_;
	//auto b_rad = ball_.getRadius()/pixel_per_meter_;

	std::cout<<"t_rad:   "<<t_rad<<"\n";
	std::cout<<"b_rad:   "<<ball_rad<<"\n";

	auto x_mid_dist = mid_ball.x - mid_paddle.x;
	auto y_mid_dist = mid_ball.y - mid_paddle.y;

	std::cout<<"x_mid_dist:   "<<x_mid_dist<<"\n";
	std::cout<<"y_mid_dist:   "<<y_mid_dist<<"\n";

	auto x_dist = x_mid_dist - ball_rad;
	auto y_dist = y_mid_dist + ball_rad;

	std::cout<<"x_dist:   "<<x_dist<<"\n";
	std::cout<<"y_dist:   "<<y_dist<<"\n";

	b_x_pos_ = x1 + x_dist;
	//b_y_pos_ = y1 + y_dist;

	std::cout<<"x_pos:   "<<b_x_pos_<<"\n";
	std::cout<<"y_pos:   "<<b_y_pos_<<"\n";

	std::cout<<"Ball: "<< ball_.getPosition().x << "  "<< ball_.getPosition().y <<"\n";
    ball_.setPosition(b_x_pos_, b_y_pos_);
    std::cout<<"Ball: "<< ball_.getPosition().x / (physical_table_size_.x / resolution_.x)  << "  "<< ball_.getPosition().y / (physical_table_size_.y / resolution_.y)<<"\n";
//}

	#else

    std::cout<<"Normal: "<<norm.x<<"  "<< norm.y<<"\n";
    std::cout<<"mid_paddle: "<<mid_paddle.x<<"  "<< mid_paddle.y<<"\n";
    std::cout<<"Ball_rad"<<ball_rad<<"\n";
    std::cout<<"Paddle_rad"<<t_rad<<"\n";

	glm::vec2 tmp = mid_paddle + norm * float(ball_rad + t_rad);
	std::cout<<mid_paddle.x << " + " << norm.x <<" * "<< ball_rad + t_rad << " = " << tmp.x <<"\n";
	std::cout<<mid_paddle.y << " + " << norm.y <<" * "<< ball_rad + t_rad << " = " << tmp.y <<"\n";
	auto new_pos = tmp + norm * glm::vec2{-ball_rad, ball_rad};

	std::cout<<tmp.x << " - " << ball_rad <<" = " << new_pos.x <<"\n";
	std::cout<<tmp.y << " + " << ball_rad <<" = " << new_pos.y <<"\n";
 
	b_x_pos_ = new_pos.x;
	//b_y_pos_ = /*physical_table_size_.y - */new_pos.y;

	std::cout<<"Ball: "<< ball_.getPosition().x << "  "<< ball_.getPosition().y<<"\n";
	std::cout<<"Ball: "<< ball_.getPosition().x*(physical_table_size_.x / resolution_.x) << "  "<< ball_.getPosition().y * (physical_table_size_.y / resolution_.y)<<"\n";

 	ball_.setPosition(b_x_pos_, b_y_pos_);
	std::cout<<"Ball: "<< ball_.getPosition().x / (physical_table_size_.x / resolution_.x)  << "  "<< ball_.getPosition().y / (physical_table_size_.y / resolution_.y)<<"\n";
	std::cout<<"Ball: "<< ball_.getPosition().x << "  "<< ball_.getPosition().y<<"\n";
	std::cout<<"new pos: "<< ball_.getPosition().x << "  "<< ball_.getPosition().y<<"\n";

	#endif


#endif

std::cout<<"pushed out\n";
}


}; //namespace TTT