#include "Table_Visualizer.h"
#include "glm/gtx/string_cast.hpp"
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

    projection_shape_.setFillColor(sf::Color::Transparent);
    projection_shape_.setOutlineColor(sf::Color::Yellow);
    projection_shape_.setOutlineThickness(line_thickness_);
    projection_shape_.setPosition(to_projection_space(physical_projection_offset_ + glm::vec2{line_thickness_ / pixel_per_projection_}));
    projection_shape_.setSize(to_projection_size(physical_projection_size_ - glm::vec2{2.0f * line_thickness_ / pixel_per_projection_}));

    table_.Recalculate_Geometry();

    for(auto& mic_obj : microphones_) {
        mic_obj.Recalculate_Geometry();
    }

    // ball_ = Ball(sf::Vector2f(ball_pos_.x, ball_pos_.y), ball_size);
    gamemode_ = true;
    ball_.setRadius(ball_size);
    ball_.setFillColor(sf::Color::Black);
    // initialize variables 
    // points_ = Score{{pixel_projection_offset_.x + pixel_projection_size_.x / 7, pixel_projection_offset_.y + pixel_projection_size_ .y * 0.5 - 10},
    // 				{pixel_projection_offset_.x + pixel_projection_size_.x * 6 / 7, pixel_projection_offset_.y + pixel_projection_size_.y * 0.5 + 10}};

    points_ = Score(6);
    ball_respawn_delay_ = 1500;
    restart();
    
}

Table_Visualizer::
~Table_Visualizer() {}

void Table_Visualizer::
Draw(sf::RenderWindow& canvas) const {

	table_.Draw(canvas);
    canvas.draw(projection_shape_);
    
    if(gamemode_ == true){
		points_.Draw(canvas);
    }


    for(auto const& mic_obj : microphones_) {
        mic_obj.Draw(canvas);
    }

    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.Draw(canvas);
    }


    // ball_.Draw(canvas);
    if(gamemode_ == true && (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_of_last_goal_).count() > ball_respawn_delay_)){
   		canvas.draw(ball_);

   	}
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

    //sf::CircleShape left  = recognized_tokens_[16000].get_Circle(); //NO HARDCODED FREQUENCIES!!!!!!!!!!!!
    //sf::CircleShape right = recognized_tokens_[18000].get_Circle();
    // sf::CircleShape t_ball = ball_.get_Circle();

    if(gamemode_ == true){
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_of_last_goal_).count() > ball_respawn_delay_){
            for(auto const& i : recognized_tokens_){

                auto intersection = ball_intersect(i.second);
                if(intersection.first){
                    
                    move_ball_out_of_token(i.second);
                    if(!move_ball_) {
                        move_ball_ = true;
                        ball_dir_ = intersection.second;
                    }
                    else {
                        ball_dir_ = glm::reflect(ball_dir_, intersection.second);
                        ball_speed_max_ *= ball_acceleration_;
                        ball_speed_min_ *= ball_acceleration_;

                    }                       
                    contact_time_ = current_time_;
                    ball_speed_ = ball_speed_max_;
                }   
            }


            float b_rad = ball_.getRadius() / pixel_per_projection_;

            glm::vec2 field_min{physical_projection_offset_ + glm::vec2{b_rad}};
            glm::vec2 field_max{physical_projection_offset_ + physical_projection_size_  - glm::vec2{b_rad}};

            // border reflection
            if(ball_pos_.x <= field_min.x || ball_pos_.x >= field_max.x){

                if(ball_pos_.x <= field_min.x){
                    ball_pos_.x = field_min.x;  
                    ball_dir_ = glm::reflect(ball_dir_, glm::vec2{1.0f ,0.0f});
                }
                else{
                    ball_pos_.x = field_max.x;
                    ball_dir_ = glm::reflect(ball_dir_, glm::vec2{-1.0f ,0.0f});
                }
            }
            // goal detection
            if(ball_pos_.y <= field_min.y - b_rad * 2.0f || ball_pos_.y >= field_max.y + b_rad * 2.0f){
                if(ball_pos_.y <= field_min.y - b_rad * 2.0f){
                    ++left_goals_;
                }
                else{
                    ++right_goals_;
                }
                points_.update(right_goals_, left_goals_);
                std::cout<<"Left: "<<left_goals_<<"  Right:  "<<right_goals_<<"\n";

                ball_dir_ = glm::vec2{0};
                ball_pos_ = physical_projection_offset_ + physical_projection_size_ * 0.5f;
                move_ball_ = false;
                ball_reset_ = true;

                time_of_last_goal_ = std::chrono::high_resolution_clock::now();

            }

            if(move_ball_ || ball_reset_) {
                if(ball_reset_) {
                    ball_reset_ = false;
                    ball_speed_ = ball_speed_min_;
                }
                else {
                    static double slowing_time = 2000;

                    // only recalculate ball speed while ball is slowing
                    if(ball_speed_ > ball_speed_min_) {
                        unsigned time_since_contact = std::chrono::duration_cast<std::chrono::milliseconds>
                        (current_time_ - contact_time_).count(); 
                        ball_speed_ = ball_speed_min_ + (ball_speed_max_ - ball_speed_min_) * (1 - double(time_since_contact) / slowing_time);
                        ball_speed_ = glm::clamp(ball_speed_, ball_speed_min_, ball_speed_max_);
                    }

                    //std::cout << ball_speed_ << std::endl;
                    ball_pos_ += ball_dir_ * float(elapsed_milliseconds_since_last_frame_ / 16.0f) * ball_speed_;
                }

    	        ball_.setPosition(to_projection_space(ball_pos_, ball_.getRadius())); 
            }
	    } 
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
    current_time_ = std::chrono::high_resolution_clock::now();

    std::chrono::milliseconds elapsed_milliseconds
        = std::chrono::duration_cast<std::chrono::milliseconds>
        (current_time_ - last_time_stamp_); 

    last_time_stamp_ = current_time_;

    elapsed_milliseconds_since_last_frame_ = elapsed_milliseconds.count();
}

unsigned Table_Visualizer::
Get_Elapsed_Milliseconds(){
    return elapsed_milliseconds_since_last_frame_;//.asMilliseconds();
}

std::pair<bool, glm::vec2> Table_Visualizer::ball_intersect(Recognized_Token_Object const& paddle) const{
    glm::vec2 mid_paddle{paddle.get_physical_position()};

    float dist = glm::length(ball_pos_ - mid_paddle);

    bool intersects = dist < (ball_.getRadius() + paddle.get_Circle().getRadius()) / pixel_per_projection_;

    glm::vec2 normal{-1.0f, 0.0f};
    if(intersects) {    
        normal = glm::normalize(ball_pos_ - mid_paddle);
    }

    return std::pair<bool, glm::vec2>{intersects, normal}; 
}

void Table_Visualizer::move_ball_out_of_token(Recognized_Token_Object const& paddle){
      
    glm::vec2 mid_paddle{paddle.get_physical_position()};
    
    glm::vec2 norm = ball_intersect(paddle).second;

    glm::vec2 new_ball_pos{mid_paddle + norm * float(ball_.getRadius() + paddle.get_Circle().getRadius()) / pixel_per_projection_};

    ball_pos_ = new_ball_pos;
}


std::pair<bool, std::string> Table_Visualizer::game_over(){
	int tmp = left_goals_ - right_goals_;
	if(abs(tmp) < points_.get_maxpoints() / 2){
		return {false, "Chuck Norris"};
	}else{
		if(tmp > 0){  //>= 3
			return {true, "Red"};
		}else{
			return {true, "Blue"};//"upper/higher"
		}
	}	
	
}



void Table_Visualizer::restart(){


	ball_pos_ = glm::vec2{physical_projection_offset_ + physical_projection_size_ * 0.5f};
    ball_dir_ = glm::vec2{0.0f, 1.0f};
    ball_speed_min_ = 0.001f;
    ball_speed_ = ball_speed_min_;
    ball_speed_max_ = 0.005f;
    ball_acceleration_ = 1.2f;
    move_ball_ = false;
    ball_reset_ = true;
    right_goals_ = 0;
    left_goals_ = 0;

	points_.reset();
    
    //get initial timestamp
    last_time_stamp_ = std::chrono::high_resolution_clock::now();

}

void Table_Visualizer::change_gm(){
	if(gamemode_ == true){
		gamemode_ = false;
	}else{
		gamemode_ = true;
	}
}


bool Table_Visualizer::wanna_play(){
	return gamemode_;
}

}; //namespace TTT
