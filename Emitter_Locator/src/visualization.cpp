#include "locator.hpp"
#include "Table_Visualizer.h"
#include "Drawable_Object.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>

#define NUM_RECORDED_CHANNELS 4

sf::Vector2f smartphonePosition(1.0f,0.5f);
sf::Vector2u windowResolution(800, 800);

//#define DEBUG_FFT_MODE

int main(int argc, char** argv) {

//XInitThreads();


sf::RenderWindow signal_plot_window_(sf::VideoMode(280, 400)
                    , "Transformed_Frequencies");

// sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();

//#ifndef DEBUG_FFT_MODE
sf::RenderWindow window(sf::VideoMode(windowResolution.x, windowResolution.y)
                       , "Table_Vis");
window.setSize(windowResolution);
//#endif

    std::vector<sf::Vector2f> default_microphone_positions_ = {{0.055, 0.08}, {0.95,  0.09}, {0.925,  1.92}, {0.105,  1.89}};


    TTT::Table_Visualizer table_visualizer(windowResolution, sf::Vector2f(1.0, 2.0), default_microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(10000);

    Locator locator{4};

    auto recording_thread = std::thread(&Locator::record_position, &locator);

    //unsigned frame_counter = 0;

        glm::vec2 max{TTT::Drawable_Object::get_phys_table_size().x, TTT::Drawable_Object::get_phys_table_size().y};
        glm::vec2 min{0, 0};
        
        glm::vec2 pl1_pos{0.01, 0.03};
        glm::vec2 pl2_pos{0.04, 0.5};

    double player_speed = 0.03;
        

        while (window.isOpen())
        {
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
               

            table_visualizer.Signal_Token(1000, sf::Vector2f(pl2_pos.x, pl2_pos.y));
            table_visualizer.Signal_Token(2000, sf::Vector2f(pl1_pos.x, pl1_pos.y));


            std::map<unsigned, std::pair<unsigned, glm::vec2> > positions = locator.load_position();
            
            if(positions.size() != 0) {
                //std::cout << "Size of map: " << positions.size();

                //for(auto const& entry : positions) {
                  //  std::cout << "Key: " << entry.first <<
                  //                "Value: " << entry.second.second.x << ", " <<  entry.second.second.y << "\n";
                //}
                //std::cin.get();
            }

            if(positions.find(18000) != positions.end()) {

                glm::vec2 current_frequency_position = positions[18000].second;

                smartphonePosition.x = current_frequency_position.x;
                smartphonePosition.y = 1.0 - current_frequency_position.y;

               // std::cout << "I Want to signal!\n";
                //std::cin.get();
                table_visualizer.Signal_Token(18000, sf::Vector2f(smartphonePosition.x, smartphonePosition.y));
            }



            table_visualizer.Finalize_Visualization_Frame();
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
                    //convex.setPoint(sample_idx, sf::Vector2f( width * sample_idx, channel_iterator * 100.0 + (100.0-sig) ));


                    if(sample_idx <  recognized_vis_sample_pos[channel_iterator] ) {
                        data_point.setFillColor(sf::Color(255, 0, 0) ) ;
                    } else {
                        data_point.setFillColor(sf::Color(0, 255, 0) ) ;          
                    }
    

                    signal_plot_window_.draw(data_point);

                }







        }

    


        signal_plot_window_.display();

        }





    locator.shut_down();
    recording_thread.join();

    return 0;
}
