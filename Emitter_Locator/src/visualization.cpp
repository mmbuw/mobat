#include "locator.hpp"
#include "Table_Visualizer.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>

#define NUM_RECORDED_CHANNELS 4

sf::Vector2f smartphonePosition(1.0f,0.5f);
sf::Vector2u windowResolution(800, 800);


int main(int argc, char** argv) {

XInitThreads();
//sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(sf::VideoMode(windowResolution.x, windowResolution.y)
                        , "Table_Vis");
//window.setSize(windowResolution);

    std::vector<sf::Vector2f> default_microphone_positions_ = {{0.055, 0.08}, {0.95,  0.09}, {0.105,  1.89}, {0.925,  1.92}};
       /* default_microphone_positions_.push_back(sf::Vector2f(0.0,4.0));
        default_microphone_positions_.push_back(sf::Vector2f(0.0,8.0));
        default_microphone_positions_.push_back(sf::Vector2f(4.0,0.0));
        default_microphone_positions_.push_back(sf::Vector2f(4.0,8.0));*/



    TTT::Table_Visualizer table_visualizer(windowResolution, sf::Vector2f(1.0, 2.0), default_microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(10000);

    Locator locator{4};

    auto recording_thread = std::thread(&Locator::record_position, &locator);

    //unsigned frame_counter = 0;
#if 1
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {

            	if(event.key.code == sf::Keyboard::Escape)
            		window.close();

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
/*
        if( 0 == ++frame_counter % 40  ) {
         table_visualizer.Signal_Token(16000, sf::Vector2f(smartphonePosition.x/2.0,
                                                            smartphonePosition.y+0.2));
        }
*/
        table_visualizer.Finalize_Visualization_Frame();
        window.display();
    }

#else //pong mit tastatur

        double x_max = windowResolution.x;
        double y_max = windowResolution.y;
        

        double l_y_pos  = 0.03;
        double r_y_pos  = 0.5;


        double l_x_pos  = 0.01;
        double r_x_pos  = 0.04/* - 2*right.getRadius()*/;


    double player_speed = 0.03;
        

        while (window.isOpen())
        {
            //sf::Event event;
            //while (window.pollEvent(event))

    /*        switch (event.type)
            {
                // window closed
                case sf::Event::Closed:
                    window.close();
                    break;*/

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if(r_y_pos > 0)
                  r_y_pos += player_speed;
                std::cout<<"Up pressed\n";
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                if(r_y_pos < y_max /*-2*right.getRadius()*/)
                    r_y_pos -= player_speed;
                std::cout<<"Down pressed\n";
            }


            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if(r_x_pos < x_max /*-2*right.getRadius()*/)
                    r_x_pos += player_speed;
                std::cout<<"Right pressed\n";
            }


            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if(r_x_pos - player_speed > x_max/2.0)
                    r_x_pos -= player_speed;
                std::cout<<"Left pressed\n";
            }

            //left player

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                if(l_y_pos > 0)
                    l_y_pos += player_speed;
                std::cout<<"W pressed\n";
            }


            if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                if(l_y_pos < y_max/*-2*left.getRadius()*/)
                    l_y_pos -= player_speed;
                std::cout<<"S pressed\n";
            }


            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                if(l_x_pos +player_speed < x_max/2.0 /*- 2*left.getRadius()*/)
                    l_x_pos += player_speed;
                std::cout<<"D pressed\n";
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                if(l_x_pos > 0)
                    l_x_pos -= player_speed;
                std::cout<<"A pressed\n";
            }
                    
            





            window.clear();
            table_visualizer.Recalculate_Geometry();



            table_visualizer.Draw(window);
               
            /*glm::vec2 point = locator.load_position();
            smartphonePosition.x = point.x;
            smartphonePosition.y = 1.0 - point.y;
*/
            //std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";


            //std::cout << l_x_pos << "; " << r_y_pos << "\n";
            table_visualizer.Signal_Token(16000, sf::Vector2f(r_x_pos, r_y_pos));
            table_visualizer.Signal_Token(18000, sf::Vector2f(l_x_pos, l_y_pos));

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
         
        }





#endif



    locator.shut_down();
    recording_thread.join();

    return 0;
}
