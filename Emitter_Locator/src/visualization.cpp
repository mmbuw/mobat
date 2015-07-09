#include "locator.hpp"
#include "Table_Visualizer.h"
#include "Drawable_Object.h"
#include "score.h"

#include <SFML/Graphics.hpp>



#include <iostream>
#include <thread>
#include <X11/Xlib.h>

#define NUM_RECORDED_CHANNELS 4

sf::Vector2f smartphonePosition(1.0f,0.5f);
glm::vec2 windowResolution(1280, 800);



int main(int argc, char** argv) {
// calculation
    Locator locator{4};

    locator.set_frequencies_to_record({19000, 17000 /*, 100000*/});

    auto recording_thread = std::thread(&Locator::record_position, &locator);

// visualisation
    //sf::RenderWindow signal_plot_window_(sf::VideoMode(280, 400)
    //                   , "Transformed_Frequencies");

#if 1



    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(unsigned(windowResolution.x), unsigned(windowResolution.y))
                           , "Table_Vis", sf::Style::Fullscreen, settings);

#else
    sf::RenderWindow window(sf::VideoMode(unsigned(windowResolution.x), unsigned(windowResolution.y))
                           , "Table_Vis");
#endif


    // Limit the framerate to 60 frames per second (this step is optional)
    window.setFramerateLimit(60);
    unsigned latest_received_timestamp = 0;


    //std::vector<glm::vec2> default_microphone_positions_ = {{0.06, 0.075}, {0.945,  0.09}, {0.925,  1.915} , {0.06,  1.905}};
    std::vector<glm::vec2> default_microphone_positions_ = {{0.057, 0.125}, {0.54,  0.12}, {0.52,  1.08} , {0.065,  1.075}};

    glm::vec2 table_dims{0.60, 1.20};
    // initialize measures for drawing & simulation 
    TTT::Drawable_Object::set_basis(glm::vec2(0,1), glm::vec2(1,0));
    // TTT::Drawable_Object::set_basis(glm::vec2(1,0), glm::vec2(0,1));
    TTT::Drawable_Object::set_resolution(windowResolution);
    TTT::Drawable_Object::set_phys_table_size(table_dims);
    TTT::Drawable_Object::set_projection(glm::vec2{0.02, 0.165}, glm::vec2{0.555, 0.885});

    TTT::Table_Visualizer table_visualizer(default_microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(5000000);

    table_visualizer.Set_Token_Fill_Color(19000, sf::Color(0,0,255) );
    table_visualizer.Set_Token_Fill_Color(17000, sf::Color(255, 0, 0) );

    //table_visualizer.Set_Token_Fill_Color(100000, sf::Color(255,0,255) );
    

// game
    std::string winner;
    glm::vec2 field_max{TTT::Drawable_Object::physical_projection_offset_ + TTT::Drawable_Object::physical_projection_size_ - glm::vec2{0.02f, 0.02f}};
    glm::vec2 field_min{TTT::Drawable_Object::physical_projection_offset_ + glm::vec2{0.02f, 0.02f}};
        
    glm::vec2 pl1_pos{TTT::Drawable_Object::physical_projection_offset_ + TTT::Drawable_Object::physical_projection_size_ * glm::vec2{0.5f, 0.6f}};
    glm::vec2 pl2_pos{TTT::Drawable_Object::physical_projection_offset_ + TTT::Drawable_Object::physical_projection_size_ * glm::vec2{0.5f, 0.3f}};

    double player_speed = 0.009;

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


        while (window.isOpen()) {
            if(window.pollEvent(event)) {
                if(event.type == sf::Event::KeyPressed){
                   if(event.key.code == sf::Keyboard::Space){
                        table_visualizer.change_gm();
                    }
                }
            }


            if(!table_visualizer.game_over().first){
                glm::vec2 pl1_dir{0, 0};
                glm::vec2 pl2_dir{0, 0};


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

                glm::vec2 pl1_new{pl1_pos + pl1_dir};
                pl1_pos = glm::clamp(pl1_new, field_min, field_max);

                 //left player

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

                


                glm::vec2 pl2_new{pl2_pos + pl2_dir};
                pl2_pos = glm::clamp(pl2_new, field_min, field_max);
                window.clear();
                table_visualizer.Recalculate_Geometry();

                table_visualizer.Draw(window);


                //table_visualizer.Signal_Token(2000, pl1_pos);
                //table_visualizer.Signal_Token(1000, pl2_pos);


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

                window.display();   


                glm::vec4 toas = locator.load_toas();
                 
                 /*
                std::array< std::vector<double>, 4> signal_vis_samples =  locator.load_signal_vis_samples();
        
                signal_plot_window_.clear(sf::Color(255, 255, 255));

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
            
                        signal_plot_window_.draw(data_point);
                    
                    }
                }

                signal_plot_window_.display();
            */
            }else{
                if(draw_endscreen_){

                    winner = table_visualizer.game_over().second;
                    //std::cout<< "Winner is: " << winner <<"\n";
                    //std::cout<<max.x << "  "<< max.y <<"\n";

                   if(winner == "Red"){
                        window.draw(rect_red);
                    }
                    else{
                        window.draw(rect_blue);
                    }

                    draw_endscreen_ = false;
                }
            


                /*sf::Color color = sf::Color(255, 192, 203);
                //window.clear();
                sf::Text text;
                sf::Text play_again;
                sf::Font font;
                font.loadFromFile("../fonds/OpenSans-Light.ttf");
                text.setFont(font);
                text.setString(winner + " wins!\n");
                text.setCharacterSize(24);
                text.setColor(color);          
                
                text.setPosition(windowResolution.x/2.5 , windowResolution.y/2.0 - 40);

                play_again.setFont(font);
                play_again.setString("If you want to play again press return!\n");
                play_again.setCharacterSize(24);
                play_again.setColor(color);          
                
                play_again.setPosition(windowResolution.x/5.0, windowResolution.y/2.0 + 20);
                */

                /*window.draw(text);
                window.draw(play_again);*/
                window.display();

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !table_visualizer.wanna_play()) {
                    draw_endscreen_ = true;
                    table_visualizer.restart();

        
                }

            }
        }

        locator.shut_down();
        recording_thread.join();
 
    return 0;
}
