#include "locator.hpp"
#include "table_visualizer.hpp"
#include "drawable_object.hpp"
#include "score.hpp"
#include "configurator.hpp"
#include "test.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>

#include <time.h>
#include <boost/filesystem.hpp>

void draw_signal_plot(sf::RenderWindow& window, Locator const& locator);

int main(int argc, char** argv) {

    std::string file_name{"default.conf"};
    if(argc > 1) {
        file_name = argv[1];
    }

    configurator().read(file_name);

    glm::vec2 windowResolution{configurator().getVec("resolution")};


    std::vector<unsigned> frequencies_to_record{configurator().getList("known_frequencies")};

// testing
    TTT::Test test_logger;
    for(auto const& i : frequencies_to_record){

        if(boost::filesystem::create_directory(std::to_string(i))){}
    }

// calculation
    Locator locator{4};

    locator.setFrequenciesToRecord(frequencies_to_record);

    auto recording_thread = std::thread(&Locator::recordPosition, &locator);

// visualisation

    bool show_signalvis = configurator().getUint("show_signalvis") > 0;
    bool show_errorvis = configurator().getUint("show_errorvis") > 0;
    bool show_tablevis = configurator().getUint("show_tablevis") > 0;

    //std::vector<unsigned> const known_frequencies = configurator().getList("known_frequencies");

    unsigned const num_audio_channels = 4;
    unsigned signal_vis_window_width  = configurator().getUint("svis_slot_width") * (frequencies_to_record.size() );
    unsigned signal_vis_window_height = configurator().getUint("svis_channel_height") * num_audio_channels;
    if(!show_signalvis) {
        signal_vis_window_height = 1;
        signal_vis_window_width = 1;
    }

    sf::RenderWindow signal_plot_window_(sf::VideoMode(signal_vis_window_width, signal_vis_window_height)
                       , "Transformed_Frequencies");

    sf::ContextSettings settings;
    //settings.antialiasingLevel = 8;

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

    TTT::TableVisualizer tisualizer(microphone_positions_);
    tisualizer.setTokenRecognitionTimeout(configurator().getUint("recognition_timeout"));

    tisualizer.setTokenFillColor(configurator().getUint("player1"), sf::Color(0,0,255) );
    tisualizer.setTokenFillColor(configurator().getUint("player2"), sf::Color(255, 0, 0) );
    
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

    bool testing = false;

    time_t starttime = time(0);
    time_t endtime = starttime +60;

    while (window.isOpen()) {
        if(window.pollEvent(event)) {
            //turn testing on and off
           if(event.key.code == sf::Keyboard::T){
                if(event.type == sf::Event::KeyReleased){
                    //get timestamp
                    starttime = time(NULL);
                    endtime = starttime +60;
                    time_t t = time(0);   // get time now
                    struct tm * now = localtime( & t );

                    std::string timestamp = 
                        std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) + "_" + 
                        std::to_string((now->tm_hour)%24) + ":" + std::to_string((now->tm_min)%60) + ":" + std::to_string((now->tm_sec)%60);
                    if(testing){
                        std::cout<<"ended logging of " << timestamp << " by pushing t\n";
                        testing = false;
                        test_logger.closeFiles();
                    } else{
                        std::cout<<"started logging of " << timestamp << "\n";
                        testing = true;
                        std::vector<std::pair<std::string, std::string>> filenames;
                        for(auto const& freq : frequencies_to_record){
                            filenames.push_back({std::to_string(freq), timestamp});
                        }
                        test_logger.openFiles(filenames);
                    }
                }
            }
            // toggle game
            else if(event.key.code == sf::Keyboard::Space){
                if (event.type == sf::Event::KeyPressed){
                    tisualizer.toggleGame();
                }
            }
        }

        if( testing && (time(NULL) >= endtime) ){
            testing = false;
            test_logger.closeFiles(); 
            std::cout<<"ended logging of " << test_logger.getTimestamp() <<" after one minute\n";
        }

        if(!tisualizer.gameOver().first){
            if(show_tablevis) {
                window.clear();

                tisualizer.handleKeyboardInput();

                tisualizer.recalculateGeometry();
                
                if(show_errorvis) {
                    tisualizer.table_.setErrorDistribution(locator.tdoator_.getErrorDistribution());
                }

                tisualizer.draw(window);
            }
            
            std::map<unsigned, std::pair<unsigned, glm::vec2> > positions = locator.loadPosition();
            
            if(positions.size() != 0) {

                unsigned current_iteration_timestamp_peak = 0;
                for(auto const& frequency_position_entry : positions ) {     

                    if(latest_received_timestamp < positions[frequency_position_entry.first].first) {
                        current_iteration_timestamp_peak = positions[frequency_position_entry.first].first;

                        tisualizer
                            .signalToken(frequency_position_entry.first, 
                                          glm::vec2(positions[frequency_position_entry.first].second.x,
                                        positions[frequency_position_entry.first].second.y));
                        if(testing){
                            test_logger.update(frequency_position_entry.first, frequency_position_entry.second.second);
                        }
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

            if(show_tablevis) {
                tisualizer.updateTokens();
            }

            if(show_signalvis) {
                draw_signal_plot(signal_plot_window_, locator);
            }
            
        }
        else {
            if(show_signalvis) {
                if(draw_endscreen_){

                    winner = tisualizer.gameOver().second;

                   if(winner == "Red"){
                        window.draw(rect_red);
                    }
                    else{
                        window.draw(rect_blue);
                    }

                    draw_endscreen_ = false;
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !tisualizer.gameActive()) {
                    draw_endscreen_ = true;
                    tisualizer.restart();
                }
            }   
        }
        
        if(show_tablevis) {
            window.display();  
        }
    }

    locator.shutdown();
    recording_thread.join();

    return 0;
}




void draw_signal_plot(sf::RenderWindow& window, Locator const& locator) {
    
    std::map<unsigned, std::array< std::vector<double>, 4> > signal_vis_samples =  locator.loadSignalVisSamples();
        
    window.clear(sf::Color(255, 255, 255));



    std::map<unsigned, std::array<unsigned, 4> >recognized_vis_sample_pos = locator.loadRecognizedVisSamplePositions();

    std::map<unsigned, std::array<std::vector<unsigned> ,4> > peak_samples_per_frequency = locator.loadPeakSamples();

    sf::RectangleShape data_point;

    unsigned draw_iteration_counter = 0;
    for(auto const& signal_of_frequency : signal_vis_samples) {
        if(signal_vis_samples.find(signal_of_frequency.first) != signal_vis_samples.end()) {
            for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {

                for(unsigned int sample_idx = 0; sample_idx < signal_vis_samples[signal_of_frequency.first][channel_iterator].size(); sample_idx+=1) {
                    unsigned int sig = signal_vis_samples[signal_of_frequency.first][channel_iterator][sample_idx];

                    float draw_slot_width = 280.0f;
                    float sample_width = 280.0f / signal_vis_samples[signal_of_frequency.first][channel_iterator].size();

                    data_point.setSize(sf::Vector2f(1,sig) );
                    data_point.setPosition( sf::Vector2f( draw_slot_width * draw_iteration_counter + sample_width * sample_idx, channel_iterator * 100.0 + (100.0-sig) ) );

                    if(sample_idx <  recognized_vis_sample_pos[signal_of_frequency.first][channel_iterator] ) {
                        data_point.setFillColor(sf::Color(255, 0, 0) ) ;
                    } else {
                        data_point.setFillColor(sf::Color(0, 255, 0) ) ;          
                    }

                    for(auto const& peak_samples_of_channel : peak_samples_per_frequency[signal_of_frequency.first][channel_iterator]) {
                        if(sample_idx -2 <= peak_samples_of_channel && sample_idx + 2 >= peak_samples_of_channel) {
                            data_point.setFillColor(sf::Color(255, 0, 255) ) ;
                            break;   
                        }
                    }

                    window.draw(data_point);
                }

            }
        }
        ++draw_iteration_counter;
    }
    
    window.display();
}
