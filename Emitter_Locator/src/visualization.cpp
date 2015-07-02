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
glm::vec2 windowResolution(800, 800);

int main(int argc, char** argv) {

    
    std::string winner;



    



    sf::RenderWindow signal_plot_window_(sf::VideoMode(280, 400)
                       , "Transformed_Frequencies");

    sf::RenderWindow window(sf::VideoMode(unsigned(windowResolution.x), unsigned(windowResolution.y))
                           , "Table_Vis");

    // Limit the framerate to 60 frames per second (this step is optional)
    window.setFramerateLimit(60);
    unsigned latest_received_timestamp = 0;


    std::vector<sf::Vector2f> default_microphone_positions_ = {{0.06, 0.075}, {0.945,  0.09}, {0.925,  1.915} , {0.06,  1.905}};



    TTT::Table_Visualizer table_visualizer(windowResolution, glm::vec2(1.0, 2.0), default_microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(5000000);


    Locator locator{4};


    locator.set_frequencies_to_record({18000, 19000, 100000});


    auto recording_thread = std::thread(&Locator::record_position, &locator);



    glm::vec2 max{TTT::Drawable_Object::get_phys_table_size().x, TTT::Drawable_Object::get_phys_table_size().y};
    glm::vec2 min{0, 0};
        
    glm::vec2 pl1_pos{0.9, 0.5};
    glm::vec2 pl2_pos{0.04, 0.5};

    double player_speed = 0.009;
        

    table_visualizer.Set_Token_Fill_Color(18000, sf::Color(255,0,0) );
    table_visualizer.Set_Token_Fill_Color(19000, sf::Color(255,255,0) );

    table_visualizer.Set_Token_Fill_Color(100000, sf::Color(255,0,255) );
    

        while (window.isOpen()) {
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
                pl1_pos = glm::clamp(pl1_new, min, max);

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
                pl2_pos = glm::clamp(pl2_new, min, max);

                window.clear();
                table_visualizer.Recalculate_Geometry();

                table_visualizer.Draw(window);


                table_visualizer.Signal_Token(1000, glm::vec2(pl2_pos.x, pl2_pos.y));
                //table_visualizer.Signal_Token(2000, sf::Vector2f(pl1_pos.x, pl1_pos.y));


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

                }


                table_visualizer.update_tokens();

                window.display();   


                glm::vec4 toas = locator.load_toas();
                 
                std::array< std::vector<double>, 4> signal_vis_samples =  locator.load_signal_vis_samples();
        
                signal_plot_window_.clear(sf::Color(255, 255, 255));

                std::array<unsigned, 4> recognized_vis_sample_pos = locator.load_recognized_vis_sample_positions();

                sf::RectangleShape data_point;
                for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {

                    for(unsigned int sample_idx = 0; sample_idx < signal_vis_samples[channel_iterator].size(); sample_idx+=5) {
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

            }else{
                winner = table_visualizer.game_over().second;
                std::cout<< "Winner is: " << winner <<"\n";


                window.clear();
                /* sf::Text text;
                sf::Font font;
                font.loadFromFile("DejaVuSans.ttf");
                text.setFont(font);
                text.setString(winner + "wins!\n If you want to play again enter again. Otherwise enter something else!");
                text.setCharacterSize(24);
                text.setColor(sf::Color::Red);          
                text.setStyle(sf::Text::Bold | sf::Text::Underlined);


                window.draw(text);*/
                window.display();

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                    table_visualizer.restart();
                }
            }
        }

        locator.shut_down();
        recording_thread.join();
 
    return 0;
}
