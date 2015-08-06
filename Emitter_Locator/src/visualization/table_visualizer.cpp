#include "table_visualizer.hpp"
#include "configurator.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace TTT {

TableVisualizer::
TableVisualizer(std::vector<glm::vec2> const& microphone_positions,
                  sf::Color const& in_table_fill_color,
                  sf::Color const& in_microphone_fill_color,
                  sf::Color const& in_token_fill_color,
                  double ball_size) : elapsed_milliseconds_since_last_frame_(0) {

    unsigned int id_counter = 0;

    if(!microphone_positions.empty()) {
        for(auto const& pos : microphone_positions) {
            microphones_.push_back(Microphone(id_counter++, pos) );
        }
    } else {
        std::vector<glm::vec2> default_microphone_positions_;
            default_microphone_positions_.push_back(glm::vec2{0.0,0.0});
            default_microphone_positions_.push_back(glm::vec2{0.0, physical_table_size_.y});
            default_microphone_positions_.push_back(glm::vec2{physical_table_size_.x,0.0});
            default_microphone_positions_.push_back(glm::vec2{physical_table_size_.x, physical_table_size_.y});

        for(auto const& pos : default_microphone_positions_) {
            microphones_.push_back(Microphone(id_counter++, pos) );
        }
    }
    
    setTableFillColor( in_table_fill_color );
    setMicrophoneFillColor( in_microphone_fill_color );

    projection_shape_.setFillColor(sf::Color::Transparent);
    projection_shape_.setOutlineColor(sf::Color::Yellow);
    projection_shape_.setOutlineThickness(line_thickness_);
    projection_shape_.setPosition(toProjectionSpace(physical_projection_offset_ + glm::vec2{line_thickness_ / pixel_per_projection_}));
    projection_shape_.setSize(toProjectionSize(physical_projection_size_ - glm::vec2{2.0f * line_thickness_ / pixel_per_projection_}));

    table_.recalculateGeometry();

    for(auto& mic_obj : microphones_) {
        mic_obj.recalculateGeometry();
    }

    // ball_ = Ball(sf::Vector2f(ball_pos_.x, ball_pos_.y), ball_size);
    gamemode_ = true;
    ball_.setRadius(ball_size);
    ball_.setFillColor(sf::Color::Black);

    points_ = Score(configurator().getUint("num_points"));
    ball_respawn_delay_ = configurator().getUint("ball_respawn_delay");
    ball_speed_limit_ = configurator().getFloat("ball_speed_limit");
    ball_acceleration_ = configurator().getFloat("ball_acceleration");
    ball_slowing_time_ = configurator().getUint("ball_slowing_time");
    // parameters for keybaord controlled game
    player1_keyboard_ = configurator().getUint("keyboard1") > 0;
    player2_keyboard_ = configurator().getUint("keyboard2") > 0;

    player_speed_ = configurator().getFloat("player_speed");

    player1_pos_ = glm::vec2{TTT::DrawableObject::physical_projection_offset_ + TTT::DrawableObject::physical_projection_size_ * glm::vec2{0.5f, 0.3f}};
    player2_pos_ = glm::vec2{TTT::DrawableObject::physical_projection_offset_ + TTT::DrawableObject::physical_projection_size_ * glm::vec2{0.5f, 0.6f}};

    restart();
    
}

TableVisualizer::
~TableVisualizer() {}

void TableVisualizer::
draw(sf::RenderWindow& canvas) const {

    table_.draw(canvas);
    canvas.draw(projection_shape_);
    
    if(gamemode_ == true){
        points_.draw(canvas);
    }


    for(auto const& mic_obj : microphones_) {
        mic_obj.draw(canvas);
    }

    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.draw(canvas);
    }


    // ball_.draw(canvas);
    if(gamemode_ == true && (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_of_last_goal_).count() > ball_respawn_delay_)){
        canvas.draw(ball_);

    }
}


void TableVisualizer::
resetMicrophones(std::vector<Microphone> const& microphones) {
    microphones_ = microphones;
}

void TableVisualizer::
recalculateGeometry() {

    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.recalculateGeometry();
    }

    if(gamemode_ == true){
        // ensures pause after goal
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_of_last_goal_).count() > ball_respawn_delay_){

            bool already_moved_out = false;
            Token last_token;
            // check for intersections with all tokens
            for(auto const& i : recognized_tokens_){

                auto intersection = ballIntersect(i.second);
                if(intersection.first){
                    last_token = i.second;
                    // dont use new move direction when still on paddle
                    if(moved_out_) {
                        moveBallOutOfToken(i.second, last_moved_dir_);
                    }
                    else {
                        last_moved_dir_ = intersection.second;
                        moveBallOutOfToken(i.second);
                    }
                    // if ball intersects with token it should move
                    if(!move_ball_) {
                        move_ball_ = true;
                        ball_dir_ = intersection.second;
                    }
                    else {
                        ball_dir_ = intersection.second;
                    }                       
                    contact_time_ = current_time_;
                    // "push"-effect
                    ball_speed_ = ball_speed_max_;

                    already_moved_out = true;
                }   
            }
            // increase speed when leaving paddle
            if(moved_out_ && !already_moved_out && ball_speed_max_ <= ball_speed_limit_) {
                ball_speed_max_ *= ball_acceleration_;
                ball_speed_min_ *= ball_acceleration_;
            }
            // ballradius in meter
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
                // prevent ball from getting stuck between wall & paddle
                if(already_moved_out) {
                    ball_dir_ = glm::normalize(ball_pos_ - last_token.getPhysicalPosition());
                    already_moved_out = false;
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
                // set ball back to mid_point
                ball_dir_ = glm::vec2{0};
                ball_pos_ = physical_projection_offset_ + physical_projection_size_ * 0.5f;
                move_ball_ = false;
                ball_reset_ = true;
                // timestamp used to ensure pause after goal
                time_of_last_goal_ = std::chrono::high_resolution_clock::now();

            }

            if(move_ball_ || ball_reset_) {
                if(ball_reset_) {
                    ball_reset_ = false;
                    ball_speed_ = ball_speed_min_;
                }
                else {
                    // only decellerate when not at paddle
                    if(!already_moved_out) {
                        // only recalculate ball speed while ball is slowing
                        if(ball_speed_ > ball_speed_min_) {
                            unsigned time_since_contact = std::chrono::duration_cast<std::chrono::milliseconds>
                            (current_time_ - contact_time_).count(); 
                            ball_speed_ = ball_speed_min_ + (ball_speed_max_ - ball_speed_min_) * (1 - double(time_since_contact) / ball_slowing_time_);
                            ball_speed_ = glm::clamp(ball_speed_, ball_speed_min_, ball_speed_max_);
                        }

                        ball_pos_ += ball_dir_ * float(elapsed_milliseconds_since_last_frame_ / 16.0f) * ball_speed_;
                    }
                }

                ball_.setPosition(toProjectionSpace(ball_pos_, ball_.getRadius())); 
            }

            if(!moved_out_ && already_moved_out){
                moved_out_ = true;
            }
            else{
                moved_out_ = false;
            }
        }
    }
}

void TableVisualizer::
setCanvasResolution(glm::vec2 const& in_resolution ) {
    resolution_ = in_resolution;
}

void TableVisualizer::
setTableFillColor( sf::Color const& in_table_fill_color ) {
    table_.setFillColor(in_table_fill_color);
}

void TableVisualizer::
setMicrophoneFillColor( sf::Color const& in_microphone_fill_color ) {

    for(auto& mic : microphones_) {
        mic.setFillColor(in_microphone_fill_color);
    }

}

void TableVisualizer::
setTokenFillColor(unsigned frequency, sf::Color const& in_token_fill_color ) {

    token_color_mapping_[frequency] = in_token_fill_color;

    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.setFillColor(in_token_fill_color);
    }
}

void TableVisualizer::
setTokenRecognitionTimeout( unsigned in_timeout_in_ms ) {
    for( auto& id_token_pair : recognized_tokens_ ) {
        id_token_pair.second.setLifeTime(in_timeout_in_ms);
    }
}

void TableVisualizer::
signalToken(unsigned int in_id, glm::vec2 const& in_position) {

    if( recognized_tokens_.end() != recognized_tokens_.find(in_id) ) {
        tokens_to_refresh_.insert(in_id);
    }

    //std::cout << "Inserting new token with id: " << in_id << "\n";
    recognized_tokens_[in_id] = Token(in_id, in_position); 

    std::map<unsigned, sf::Color>::iterator token_color_mapping_iterator = 
        token_color_mapping_.find(in_id);
    if(token_color_mapping_.end() != token_color_mapping_.find(in_id) ) {
        recognized_tokens_[in_id]
            .setFillColor(token_color_mapping_iterator->second);
    }
}

void TableVisualizer::
updateTokens() {
    Calculate_Elapsed_Milliseconds();
    unsigned int Elapsed_Milliseconds = getElapsedMilliseconds();
    
    std::set<unsigned> tokens_to_remove;

    for( auto& id_token_pair : recognized_tokens_ ) {

        bool refresh_token = false;
        if(tokens_to_refresh_.end() != 
           tokens_to_refresh_.find(id_token_pair.first) ) {
            refresh_token = true;
        }


        
        if( ! id_token_pair.second
            .update(refresh_token, 
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


void TableVisualizer::
Calculate_Elapsed_Milliseconds() {
    current_time_ = std::chrono::high_resolution_clock::now();

    std::chrono::milliseconds elapsed_milliseconds
        = std::chrono::duration_cast<std::chrono::milliseconds>
        (current_time_ - last_time_stamp_); 

    last_time_stamp_ = current_time_;

    elapsed_milliseconds_since_last_frame_ = elapsed_milliseconds.count();
}

unsigned TableVisualizer::
getElapsedMilliseconds(){
    return elapsed_milliseconds_since_last_frame_;//.asMilliseconds();
}

std::pair<bool, glm::vec2> TableVisualizer::ballIntersect(Token const& paddle) const{
    glm::vec2 mid_paddle{paddle.getPhysicalPosition()};

    float dist = glm::length(ball_pos_ - mid_paddle);

    bool intersects = dist < (ball_.getRadius() + paddle.getCircle().getRadius() + paddle.getCircle().getOutlineThickness()) / pixel_per_projection_;

    glm::vec2 normal{-1.0f, 0.0f};
    if(intersects) {    
        normal = glm::normalize(ball_pos_ - mid_paddle);
    }

    return std::pair<bool, glm::vec2>{intersects, normal}; 
}

void TableVisualizer::moveBallOutOfToken(Token const& paddle, glm::vec2 const& dir){
      
    glm::vec2 mid_paddle{paddle.getPhysicalPosition()};
    
    glm::vec2 normal = dir;
    if(dir == glm::vec2{0.0f}) {
       normal = ballIntersect(paddle).second;
    }    

    glm::vec2 new_ball_pos{mid_paddle + normal * float(ball_.getRadius() + paddle.getCircle().getRadius() + paddle.getCircle().getOutlineThickness()) * 1.01f / pixel_per_projection_};

    ball_pos_ = new_ball_pos;
}


std::pair<bool, std::string> TableVisualizer::gameOver(){
    int tmp = left_goals_ - right_goals_;
    if(abs(tmp) < points_.getMaxPoints() / 2){
        return {false, "Chuck Norris"};
    }else{
        if(tmp > 0){  //>= 3
            return {true, "Red"};
        }else{
            return {true, "Blue"};//"upper/higher"
        }
    }   
    
}



void TableVisualizer::restart(){


    ball_pos_ = glm::vec2{physical_projection_offset_ + physical_projection_size_ * 0.5f};
    ball_dir_ = glm::vec2{0.0f, 1.0f};
    ball_speed_min_ = configurator().getFloat("ball_speed_min");
    ball_speed_ = ball_speed_min_;
    ball_speed_max_ = configurator().getFloat("ball_speed_max");
    move_ball_ = false;
    ball_reset_ = true;
    right_goals_ = 0;
    left_goals_ = 0;

    points_.reset();
    
    //get initial timestamp
    last_time_stamp_ = std::chrono::high_resolution_clock::now();

}

void TableVisualizer::toggleGame(){
    if(gamemode_ == true){
        gamemode_ = false;
    }else{
        gamemode_ = true;
    }
}


bool TableVisualizer::gameActive(){
    return gamemode_;
}


void TableVisualizer::handleKeyboardInput() {
    glm::vec2 field_max{TTT::DrawableObject::physical_projection_offset_ + TTT::DrawableObject::physical_projection_size_ - glm::vec2{0.02f, 0.02f}};
    glm::vec2 field_min{TTT::DrawableObject::physical_projection_offset_ + glm::vec2{0.02f, 0.02f}};
        
    if(player1_keyboard_) {
        glm::vec2 pl1_dir{0, 0};
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            pl1_dir.y += player_speed_;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            pl1_dir.y -= player_speed_;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            pl1_dir.x += player_speed_;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            pl1_dir.x -= player_speed_;
        }

        player1_pos_ = glm::clamp(player1_pos_ + pl1_dir, field_min, field_max);
        signalToken(configurator().getUint("player1"), player1_pos_);
    }
     //left player
    if(player2_keyboard_) {
        glm::vec2 pl2_dir{0, 0};

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pl2_dir.y += player_speed_;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pl2_dir.y -= player_speed_;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            pl2_dir.x += player_speed_;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            pl2_dir.x -= player_speed_;
        }

        player2_pos_ = glm::clamp(player2_pos_ + pl2_dir, field_min, field_max);
        signalToken(configurator().getUint("player2"), player2_pos_);
    }
}


void TableVisualizer::writeTokens() {
    for(auto const& i : recognized_tokens_){
        std::cout<<"Token " << i.first <<" at: (" << i.second.physical_position_.x << ", " << i.second.physical_position_.y << "), ";
    }
    std::cout<<"\n";
}

}; //namespace TTT
