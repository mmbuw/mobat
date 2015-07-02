#include "Table_Visualizer.h"

#include <iostream>

namespace TTT {

Table_Visualizer::
Table_Visualizer(std::vector<glm::vec2> const& microphone_positions,
                  sf::Color const& in_table_fill_color,
                  sf::Color const& in_microphone_fill_color,
                  sf::Color const& in_token_fill_color,
                  double ball_size) : elapsed_milliseconds_since_last_frame_(0) {

    unsigned int id_counter = 0;

    if(!microphone_positions.empty()) {
        for(auto const& pos : microphone_positions) {
            microphones_.push_back(Microphone_Object(id_counter++, pos) );
        }
    } else {
        std::vector<glm::vec2> default_microphone_positions_;
            default_microphone_positions_.push_back(glm::vec2{0.0,0.0});
            default_microphone_positions_.push_back(glm::vec2{0.0, physical_table_size_.y});
            default_microphone_positions_.push_back(glm::vec2{physical_table_size_.x,0.0});
            default_microphone_positions_.push_back(glm::vec2{physical_table_size_.x, physical_table_size_.y});

        for(auto const& pos : default_microphone_positions_) {
            microphones_.push_back(Microphone_Object(id_counter++, pos) );
        }
    }
    
    Set_Table_Fill_Color( in_table_fill_color );
    Set_Microphone_Fill_Color( in_microphone_fill_color );

    projection_shape_.setPosition(to_projection_space(physical_projection_offset_));
    projection_shape_.setSize(to_projection_size(projection_size_));
    projection_shape_.setFillColor(sf::Color::Yellow);

    table_.Recalculate_Geometry();

    for(auto& mic_obj : microphones_) {
        mic_obj.Recalculate_Geometry();
    }

    ball_ = Ball(sf::Vector2f(ball_pos_.x, ball_pos_.y), ball_size);
    ball_.Set_Fill_Color(sf::Color::Blue);
    // initialize variables 
    points_ = Score{{pixel_table_offset_.x + pixel_table_size_.x / 7, pixel_table_offset_.y + pixel_table_size_ .y * 0.5 - 10},
    				{pixel_table_offset_.x + pixel_table_size_.x * 6 / 7, pixel_table_offset_.y + pixel_table_size_.y * 0.5 + 10}};
    restart();
    
}

Table_Visualizer::
~Table_Visualizer() {}

void Table_Visualizer::
Draw(sf::RenderWindow& canvas) const {

	
	table_.Draw(canvas);
    canvas.draw(projection_shape_);
	points_.Draw(canvas);


    for(auto const& mic_obj : microphones_) {
        mic_obj.Draw(canvas);
    }

    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.Draw(canvas);
    }


    ball_.Draw(canvas);

}


void Table_Visualizer::
Reset_Microphones(std::vector<Microphone_Object> const& microphones) {
    microphones_ = microphones;
}

void Table_Visualizer::
Recalculate_Geometry() {

    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.Recalculate_Geometry();
    }

// pseudopong zeug



    // if(recognized_tokens_.size() >= 2){

    sf::CircleShape left  = recognized_tokens_[16000].get_Circle(); //NO HARDCODED FREQUENCIES!!!!!!!!!!!!
    sf::CircleShape right = recognized_tokens_[18000].get_Circle();
    sf::CircleShape t_ball = ball_.get_Circle();

    for(auto const& i : recognized_tokens_){

        auto token = i.second.get_Circle();
        auto intersection = circ_circ_intersect(t_ball, token);
        if(intersection.first){
            
            move_ball_out_of_token(token);
            if(!move_ball_) {
                move_ball_ = true;
                ball_dir_ = intersection.second;
            }
            else {
                ball_dir_ = glm::reflect(ball_dir_, intersection.second);
                ball_speed_ *= ball_acceleration_;

            }                       
        }   
    }


    float b_rad = ball_.getRadius();

    glm::vec2 table_min{pixel_table_offset_ + glm::vec2{b_rad}};
    glm::vec2 table_max{pixel_table_offset_ + pixel_table_size_ - glm::vec2{b_rad}};
    // border reflection
    if(ball_pos_.x <= table_min.x || ball_pos_.x >= table_max.x){

        if(ball_pos_.x <= table_min.x){
            ball_pos_.x = table_min.x;  
            ball_dir_ = glm::reflect(ball_dir_, glm::vec2{1.0f ,0.0f});
        }
        else{
            ball_pos_.x = table_max.x;
            ball_dir_ = glm::reflect(ball_dir_, glm::vec2{-1.0f ,0.0f});
        }
    }
    // goal detection
    if(ball_pos_.y <= table_min.y - b_rad * 2.0f || ball_pos_.y >= table_max.y + b_rad * 2.0f){
        if(ball_pos_.y <= table_min.y - b_rad * 2.0f){
            ++left_goals_;
        }
        else{
            ++right_goals_;
        }
        points_.update(right_goals_, left_goals_);
        std::cout<<"Left: "<<left_goals_<<"  Right:  "<<right_goals_<<"\n";

        ball_dir_ = glm::vec2{0};
        ball_pos_ = pixel_table_offset_ + pixel_table_size_* 0.5f;
        move_ball_ = false;
        ball_reset_ = true;
    }

    if(move_ball_ || ball_reset_) {
        if(ball_reset_) {
            ball_reset_ = false;
            ball_speed_ = ball_speed_min_;
        }
        else {
            ball_speed_ = glm::clamp(ball_speed_, ball_speed_min_, ball_speed_max_);
            ball_pos_ += ball_dir_ * float(elapsed_milliseconds_since_last_frame_ / 16.0f) * ball_speed_;
        }

        ball_.setPosition(ball_pos_.x, ball_pos_.y);
    } 

// }
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
Signal_Token(unsigned int in_id, glm::vec2 const& in_position) {

    if( recognized_tokens_.end() != recognized_tokens_.find(in_id) ) {
        tokens_to_refresh_.insert(in_id);
    }

    //std::cout << "Inserting new token with id: " << in_id << "\n";
    recognized_tokens_[in_id] = Recognized_Token_Object(in_id, in_position); 

    std::map<unsigned, sf::Color>::iterator token_color_mapping_iterator = 
        token_color_mapping_.find(in_id);
    if(token_color_mapping_.end() != token_color_mapping_.find(in_id) ) {
        recognized_tokens_[in_id]
            .Set_Fill_Color(token_color_mapping_iterator->second);
    }
}

void Table_Visualizer::
update_tokens() {
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

    elapsed_milliseconds_since_last_frame_ = elapsed_milliseconds.count();
}

unsigned Table_Visualizer::
Get_Elapsed_Milliseconds(){
    return elapsed_milliseconds_since_last_frame_;//.asMilliseconds();
}

std::pair<bool, glm::vec2> Table_Visualizer::circ_circ_intersect(sf::CircleShape const& ball, sf::CircleShape const& paddle) const{
    glm::vec2 mid_ball{ball.getPosition().x + ball.getRadius(), ball.getPosition().y + ball.getRadius()};
    glm::vec2 mid_paddle{paddle.getPosition().x + paddle.getRadius(), paddle.getPosition().y + paddle.getRadius()};

    float dist = glm::length(mid_ball - mid_paddle);

    bool intersects = dist < ball.getRadius() + paddle.getRadius();

    glm::vec2 normal{-1.0f, 0.0f};
    if(intersects) {    
        normal = glm::normalize(mid_ball - mid_paddle);
    }

    return std::pair<bool, glm::vec2>{intersects, normal}; 
}

void Table_Visualizer::move_ball_out_of_token(sf::CircleShape const& paddle){
      
    glm::vec2 mid_ball{ball_.getPosition().x + ball_.getRadius(), ball_.getPosition().y + ball_.getRadius()};
    glm::vec2 mid_paddle{paddle.getPosition().x + paddle.getRadius(), paddle.getPosition().y + paddle.getRadius()};
    
    glm::vec2 norm = circ_circ_intersect(ball_.get_Circle(), paddle).second;

    glm::vec2 new_ball_pos{mid_paddle + norm * float(ball_.getRadius() + paddle.getRadius())};

    ball_pos_ = new_ball_pos;
}


std::pair<bool, std::string> Table_Visualizer::game_over(){
	int tmp = left_goals_ - right_goals_;
	if(abs(tmp) < 3){
		return {false, "Chuck Norris"};
	}else{

		if(tmp >= 3){
			return {true, "Red"};
		}else{
			return {true, "Blue"};//"upper/higher"
		}
	}	
	
}



void Table_Visualizer::restart(){


	ball_pos_ = glm::vec2{pixel_table_offset_ + pixel_table_size_ * 0.5f};
    ball_dir_ = glm::vec2{0.0f, 1.0f};
    ball_speed_min_ = 2.0f;
    ball_speed_ = ball_speed_min_;
    ball_speed_max_ = 5.0f;
    ball_acceleration_ = 1.2f;
    move_ball_ = false;
    ball_reset_ = true;
    right_goals_ = 0;
    left_goals_ = 0;

	points_.reset();
    
    //get initial timestamp
    last_time_stamp_ = std::chrono::high_resolution_clock::now();

}

}; //namespace TTT
