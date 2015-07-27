#include "locator.hpp"
#include "table_visualizer.hpp"
#include "drawable_object.hpp"
#include "score.hpp"
#include "configurator.hpp"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>

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
    TTT::DrawableObject::setBasis(configurator().getVec("right"), configurator().getVec("up"));
    TTT::DrawableObject::setResolution(windowResolution);
    TTT::DrawableObject::setPhysTableSize(configurator().getVec("table_size"));
    TTT::DrawableObject::setProjection(configurator().getVec("projection_offset"), configurator().getVec("projection_size"));

    TTT::TableVisualizer TableVisualizer(microphone_positions_);
    TableVisualizer.setTokenRecognitionTimeout(configurator().getUint("recognition_timeout"));

    TableVisualizer.setTokenFillColor(configurator().getUint("player1"), sf::Color(0,0,255) );
    TableVisualizer.setTokenFillColor(configurator().getUint("player2"), sf::Color(255, 0, 0) );
    
// game
    std::string winner;

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
                    TableVisualizer.toggleGame();

                }
            }
        }

        if(!TableVisualizer.gameOver().first){
            window.clear();

            TableVisualizer.handleKeyboardInput();

            TableVisualizer.recalculateGeometry();

            TableVisualizer.draw(window);

            std::map<unsigned, std::pair<unsigned, glm::vec2> > positions = locator.load_position();
            
            if(positions.size() != 0) {

                unsigned current_iteration_timestamp_peak = 0;
                for(auto const& frequency_position_entry : positions ) {     

                    if(latest_received_timestamp < positions[frequency_position_entry.first].first) {
                        current_iteration_timestamp_peak = positions[frequency_position_entry.first].first;

                        TableVisualizer
                            .signalToken(frequency_position_entry.first, 
                                          glm::vec2(positions[frequency_position_entry.first].second.x,
                                           1.0 - positions[frequency_position_entry.first].second.y));
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

            TableVisualizer.updateTokens();
            
            if(show_signalvis) {
                draw_signal_plot(signal_plot_window_, locator);
            }
            
        }
        else {

            if(draw_endscreen_){

                winner = TableVisualizer.gameOver().second;

               if(winner == "Red"){
                    window.draw(rect_red);
                }
                else{
                    window.draw(rect_blue);
                }

                draw_endscreen_ = false;
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !TableVisualizer.gameActive()) {
                draw_endscreen_ = true;
                TableVisualizer.restart();
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
