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

#define DEBUG_FFT_MODE

int main(int argc, char** argv) {

//XInitThreads();


sf::RenderWindow signal_plot_window_(sf::VideoMode(1400, 400)
                    , "Transformed_Frequencies");

// sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();

//#ifndef DEBUG_FFT_MODE
sf::RenderWindow window(sf::VideoMode(windowResolution.x, windowResolution.y)
                       , "Table_Vis");
window.setSize(windowResolution);
//#endif

    std::vector<sf::Vector2f> default_microphone_positions_ = {{0.055, 0.08}, {0.95,  0.09}, {0.925,  1.92}, {0.105,  1.89}};
       /* default_microphone_positions_.push_back(sf::Vector2f(0.0,4.0));
        default_microphone_positions_.push_back(sf::Vector2f(0.0,8.0));
        default_microphone_positions_.push_back(sf::Vector2f(4.0,0.0));
        default_microphone_positions_.push_back(sf::Vector2f(4.0,8.0));*/



    TTT::Table_Visualizer table_visualizer(windowResolution, sf::Vector2f(1.0, 2.0), default_microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(10000);

    Locator locator{4};

    auto recording_thread = std::thread(&Locator::record_position, &locator);

    //unsigned frame_counter = 0;
#ifdef DEBUG_FFT_MODE
    


    while (window.isOpen())
    {

        std::array< std::vector<unsigned int>, 4> signal_vis_samples =  locator.load_signal_vis_samples();
        signal_plot_window_.clear(sf::Color(255, 255, 255));

        std::array<unsigned, 4> recognized_vis_sample_pos = locator.load_recognized_vis_sample_positions();

        


        for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {


                //for(auto const& sig : signal_vis_samples[channel_iterator]) {

                sf::ConvexShape polyLine(signal_vis_samples[channel_iterator].size());
                polyLine.setFillColor(sf::Color(255,0,0));
                for(unsigned int sample_idx = 0; sample_idx < signal_vis_samples[channel_iterator].size(); sample_idx+=1) {
                    unsigned int sig = signal_vis_samples[channel_iterator][sample_idx];

                    float width = 1400.0f / signal_vis_samples[channel_iterator].size();

                    sf::RectangleShape data_point(sf::Vector2f(1,sig) );
                    data_point.setPosition( sf::Vector2f( width * sample_idx, channel_iterator * 100.0 + (100.0-sig) ) );



                    if(sample_idx <  recognized_vis_sample_pos[channel_iterator] ) {
                        data_point.setFillColor(sf::Color(255, 0, 0) ) ;
                    } else {
                        data_point.setFillColor(sf::Color(0, 255, 0) ) ;          
                    }
    

                    signal_plot_window_.draw(data_point);

                   // std::cout << "Sig: " << sig << "\n";



         
                }


        }

    


        signal_plot_window_.display();
       // std::cout << "Started loop\n";
        
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                    exit(1);
        //        window.close();

            if (event.type == sf::Event::KeyPressed) {

          //  	if(event.key.code == sf::Keyboard::Escape)
          //  		window.close();

            	if(event.key.code == sf::Keyboard::Up 
            		|| event.key.code == sf::Keyboard::Down
            		|| event.key.code == sf::Keyboard::Left 
            		|| event.key.code == sf::Keyboard::Right) {
	            	// if (event.key.code == sf::Keyboard::Up) {

              //           if(current_listened_channel < 3)
              //               ++current_listened_channel;
	            	// }
	            	// if (event.key.code == sf::Keyboard::Down) {

              //           if(current_listened_channel > 0)
              //               --current_listened_channel;
	            	// }
	            	if (event.key.code == sf::Keyboard::Left) {
	            	} 
	            	if (event.key.code == sf::Keyboard::Right) {
	            	}

                }
               
            }
       }
           
        glm::vec2 point = locator.load_position();
        smartphonePosition.x = point.x;
        smartphonePosition.y = 1.0 - point.y;


        glm::vec4 toas = locator.load_toas();



        window.clear();
        table_visualizer.Recalculate_Geometry();
        table_visualizer.Draw(window, toas);

   //    std::cout << "toas: " << toas[0] << ", " << toas[1] << ", " << toas[2] << ", " << toas[3] << "\n";

        //std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";
        table_visualizer.Signal_Token(16000, smartphonePosition);
        table_visualizer.Signal_Token(18000, smartphonePosition);

        /*if( 0 == ++frame_counter % 40  ) {
         table_visualizer.Signal_Token(16000, sf::Vector2f(smartphonePosition.x/2.0,
                                                            smartphonePosition.y+0.2));
        }*/

        table_visualizer.Finalize_Visualization_Frame();
        window.display();
    }

#else //pong mit tastatur
        glm::vec2 max{TTT::Drawable_Object::get_phys_table_size().x, TTT::Drawable_Object::get_phys_table_size().y};
        glm::vec2 min{0, 0};
        
        glm::vec2 pl1_pos{0.01, 0.03};
        glm::vec2 pl2_pos{0.04, 0.5};

    double player_speed = 0.03;
        

        while (window.isOpen())
        {
            glm::vec2 pl1_dir{0, 0};
            glm::vec2 pl2_dir{0, 0};
            //sf::Event event;
            //while (window.pollEvent(event))

    /*        switch (event.type)
            {
                // window closed
                case sf::Event::Closed:
                    window.close();
                    break;*/

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
               
            /*glm::vec2 point = locator.load_position();
            smartphonePosition.x = point.x;
            smartphonePosition.y = 1.0 - point.y;
*/
            //std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";


            //std::cout << pl1_pos.x << "; " << pl2_pos.y << "\n";
            table_visualizer.Signal_Token(16000, sf::Vector2f(pl2_pos.x, pl2_pos.y));
            table_visualizer.Signal_Token(18000, sf::Vector2f(pl1_pos.x, pl1_pos.y));

    /*
            if( 0 == ++frame_counter % 40  ) {
             table_visualizer.Signal_Token(16000, sf::Vector2f(smartphonePosition.x/2.0,
                                                                 smartphonePosition.y+0.2));
            }
    */
            table_visualizer.Finalize_Visualization_Frame();
            window.display();

                  
                    /*glm::vec2 point = locator.load_position();
                    smartphonePosition.x = point.x;
                    smartphonePosition.y = 1.0 - point.y;

                    //std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";
                    table_visualizer.Signal_Token(18000, smartphonePosition);*/

            /*
                    if( 0 == ++frame_counter % 40  ) {
                     table_visualizer.Signal_Token(16000, sf::Vector2f(smartphonePosition.x/2.0,
                                                                        smartphonePosition.y+0.2));
                    }
            */
                    /*table_visualizer.Finalize_Visualization_Frame();
                    window.display();*/


                            glm::vec2 point = locator.load_position();
        smartphonePosition.x = point.x;
        smartphonePosition.y = 1.0 - point.y;


        glm::vec4 toas = locator.load_toas();
         
        }





#endif



    locator.shut_down();
    recording_thread.join();

    return 0;
}
