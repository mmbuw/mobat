#include "locator.hpp"
#include "Table_Visualizer.h"
#include "Drawable_Object.h"
#include "score.h"
#include "configurator.hpp"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>

#define NUM_RECORDED_CHANNELS 4

sf::Vector2f smartphonePosition(1.0f,0.5f);

void draw_signal_plot(sf::RenderWindow& window, Locator const& locator);

int main(int argc, char** argv) {

    std::string file_name{"default.conf"};
    if(argc > 1) {
        file_name = argv[1];
    }

    configurator().read(file_name);

    glm::vec2 windowResolution{configurator().getVec("resolution")};

// calculation
    Locator locator{4};

    locator.set_frequencies_to_record({configurator().getUint("player1"), configurator().getUint("player2") /*, 100000*/});

    auto recording_thread = std::thread(&Locator::record_position, &locator);

// visualisation

    bool show_signalvis = configurator().getUint("show_signalvis") > 0;

    sf::RenderWindow signal_plot_window_(sf::VideoMode(280, 400)
                       , "Transformed_Frequencies");

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    
    sf::Uint32 style = sf::Style::Default;
    if(configurator().getUint("fullscreen") > 0) {
        style = sf::Style::Fullscreen;
    }

    sf::RenderWindow window(sf::VideoMode(unsigned(windowResolution.x), unsigned(windowResolution.y))
                           , "Table_Vis", style, settings);

    // Limit the framerate to 60 frames per second (this step is optional)
    window.setFramerateLimit(60);
    unsigned latest_received_timestamp = 0;

    std::vector<glm::vec2> microphone_positions_ = {configurator().getVec("microphone1_pos"),
                                                    configurator().getVec("microphone2_pos"),
                                                    configurator().getVec("microphone3_pos"),
                                                    configurator().getVec("microphone4_pos")};

    // initialize measures for drawing & simulation 
    TTT::Drawable_Object::set_basis(configurator().getVec("right"), configurator().getVec("up"));
    TTT::Drawable_Object::set_resolution(windowResolution);
    TTT::Drawable_Object::set_phys_table_size(configurator().getVec("table_size"));
    TTT::Drawable_Object::set_projection(configurator().getVec("projection_offset"), configurator().getVec("projection_size"));

    TTT::Table_Visualizer table_visualizer(microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(configurator().getUint("recognition_timeout"));

    table_visualizer.Set_Token_Fill_Color(configurator().getUint("player1"), sf::Color(0,0,255) );
    table_visualizer.Set_Token_Fill_Color(configurator().getUint("player2"), sf::Color(255, 0, 0) );
    
// game
    std::string winner;
    glm::vec2 field_max{TTT::Drawable_Object::physical_projection_offset_ + TTT::Drawable_Object::physical_projection_size_ - glm::vec2{0.02f, 0.02f}};
    glm::vec2 field_min{TTT::Drawable_Object::physical_projection_offset_ + glm::vec2{0.02f, 0.02f}};
        
    glm::vec2 pl1_pos{TTT::Drawable_Object::physical_projection_offset_ + TTT::Drawable_Object::physical_projection_size_ * glm::vec2{0.5f, 0.3f}};
    glm::vec2 pl2_pos{TTT::Drawable_Object::physical_projection_offset_ + TTT::Drawable_Object::physical_projection_size_ * glm::vec2{0.5f, 0.6f}};

    float player_speed = configurator().getFloat("player_speed");

    sf::Event event;
        
    sf::Texture texture_red;
    texture_red.loadFromFile("../pictures/red_wins.png"); 
    sf::Texture texture_blue;
    texture_blue.loadFromFile("../pictures/blue_wins.png"); 

    sf::RectangleShape rect_red;
    rect_red.setSize(sf::Vector2f(600, 400));
    rect_red.setPosition(sf::Vector2f(windowResolution.x/2 - 300, windowResolution.y/2 - 200));
    rect_red.setTexture(&texture_red, false);
    sf::RectangleShape rect_blue{rect_red};
    rect_blue.setTexture(&texture_blue, false);

    bool draw_endscreen_ = true;

    bool player1_keyboard = configurator().getUint("keyboard1") > 0;
    bool player2_keyboard = configurator().getUint("keyboard2") > 0;

    while (window.isOpen()) {
        if(window.pollEvent(event)) {
            if(event.type == sf::Event::KeyPressed){
               if(event.key.code == sf::Keyboard::Space){
                    table_visualizer.change_gm();

                }
            }
        }

        if(!table_visualizer.game_over().first){
            window.clear();

            if(player1_keyboard) {
                glm::vec2 pl1_dir{0, 0};
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    pl1_dir.y += player_speed;
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    pl1_dir.y -= player_speed;
                }


                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    pl1_dir.x += player_speed;
                }


                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    pl1_dir.x -= player_speed;
                }

                pl1_pos = glm::clamp(pl1_pos + pl1_dir, field_min, field_max);
                table_visualizer.Signal_Token(configurator().getUint("player1"), pl1_pos);
            }
             //left player
            if(player2_keyboard) {
                glm::vec2 pl2_dir{0, 0};

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                    pl2_dir.y += player_speed;
                }


                if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    pl2_dir.y -= player_speed;
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                    pl2_dir.x += player_speed;
                }


                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                    pl2_dir.x -= player_speed;
                }

                pl2_pos = glm::clamp(pl2_pos + pl2_dir, field_min, field_max);
                table_visualizer.Signal_Token(configurator().getUint("player2"), pl2_pos);
            }

            table_visualizer.Recalculate_Geometry();

            table_visualizer.Draw(window);

            std::map<unsigned, std::pair<unsigned, glm::vec2> > positions = locator.load_position();
            
            if(positions.size() != 0) {
                //std::cout << "Started getting posses: \n";

                unsigned current_iteration_timestamp_peak = 0;
                for(auto const& frequency_position_entry : positions ) {     


                    if(latest_received_timestamp < positions[frequency_position_entry.first].first) {
                        current_iteration_timestamp_peak = positions[frequency_position_entry.first].first;

                        glm::vec2 current_frequency_position = positions[frequency_position_entry.first].second;

                        smartphonePosition.x = current_frequency_position.x;
                        smartphonePosition.y = 1.0 - current_frequency_position.y;

                        table_visualizer
                            .Signal_Token(frequency_position_entry.first, 
                                          glm::vec2(smartphonePosition.x, 
                                                       smartphonePosition.y));
                    }
                }

                if(current_iteration_timestamp_peak > latest_received_timestamp) {
                    latest_received_timestamp = current_iteration_timestamp_peak;
                }

            } else {
                //for(auto const& frequency_position_entry : positions ) {
                    //positions_to_average[frequency_position_entry][vis_frame_count % 10] 
                    //= std::make_pair(false, glm::vec2(0.0, 0.0));
                //}
            }

            table_visualizer.update_tokens();
            
            if(show_signalvis) {
                draw_signal_plot(signal_plot_window_, locator);
            }
            
        }
        else {

            if(draw_endscreen_){

                winner = table_visualizer.game_over().second;

               if(winner == "Red"){
                    window.draw(rect_red);
                }
                else{
                    window.draw(rect_blue);
                }

                draw_endscreen_ = false;
            }
        

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !table_visualizer.wanna_play()) {
                draw_endscreen_ = true;
                table_visualizer.restart();
            }
        }
        
        window.display();  
    }

    locator.shut_down();
    recording_thread.join();

    return 0;
}




void draw_signal_plot(sf::RenderWindow& window, Locator const& locator) {
    std::array< std::vector<double>, 4> signal_vis_samples =  locator.load_signal_vis_samples();
        
    window.clear(sf::Color(255, 255, 255));


    std::array<unsigned, 4> recognized_vis_sample_pos = locator.load_recognized_vis_sample_positions();

    sf::RectangleShape data_point;
    for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {

        for(unsigned int sample_idx = 0; sample_idx < signal_vis_samples[channel_iterator].size(); sample_idx+=1) {
            unsigned int sig = signal_vis_samples[channel_iterator][sample_idx];


            float width = 280.0f / signal_vis_samples[channel_iterator].size();

            data_point.setSize(sf::Vector2f(1,sig) );
            data_point.setPosition( sf::Vector2f( width * sample_idx, channel_iterator * 100.0 + (100.0-sig) ) );

            if(sample_idx <  recognized_vis_sample_pos[channel_iterator] ) {
                data_point.setFillColor(sf::Color(255, 0, 0) ) ;
            } else {
                data_point.setFillColor(sf::Color(0, 255, 0) ) ;          
            }

            window.draw(data_point);
        }
    }

    window.display();
}
