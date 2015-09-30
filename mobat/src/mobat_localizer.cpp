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

#include <termios.h>

void draw_signal_plot(sf::RenderWindow& window, Locator const& locator);

#define NB_ENABLE 1
#define NB_DISABLE 0

bool logging = false;

unsigned latest_received_timestamp = 0;
time_t starttime = time(0);
time_t endtime = starttime +60;

// testing
TTT::Test test_logger;


void sendPositions( std::map<unsigned, std::pair<unsigned, glm::vec2> > const& located_positions, 
                    unsigned& frontier_timestamp );
void toggleLogging( std::vector<unsigned> frequencies_to_log );
/*
  functions "kbhit" and "nonblock" taken from: 
  http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
  (Accessed: 30th September 2015)
*/

int kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state) {
    struct termios ttystate;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    if (state==NB_ENABLE)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==NB_DISABLE)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
 
}

int main(int argc, char** argv) {


    std::string file_name{"default.conf"};
    if(argc > 1) {
        file_name = argv[1];
    }

    configurator().read(file_name);

    glm::vec2 windowResolution{configurator().getVec("resolution")};


    std::vector<unsigned> frequencies_to_record{configurator().getList("known_frequencies")};



// calculation
    Locator locator{4};

    locator.setFrequenciesToRecord(frequencies_to_record);

    auto recording_thread = std::thread(&Locator::recordPosition, &locator);

    // visualisation

    bool show_signalvis = configurator().getUint("show_signalvis") > 0;

    //logging auxiliary
    for(auto const& i : frequencies_to_record){

        if(boost::filesystem::create_directory(std::to_string(i))){}
    }

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




    std::vector<glm::vec2> microphone_positions_ = {configurator().getVec("microphone1_pos"),
                                                    configurator().getVec("microphone2_pos"),
                                                    configurator().getVec("microphone3_pos"),
                                                    configurator().getVec("microphone4_pos")};

    
// game
    //sf::Event event;





    //while (window.isOpen()) {
 /*   while( true ) {
*/
    char c = '0';
    char prev_character = '0';
    int i=0;
 
    nonblock(NB_ENABLE);
    while( true ) {
        usleep(1);
        i=kbhit();

        if (i!=0)
        {
            prev_character = c;
            c=fgetc(stdin);
            if (c == 27) {
                i=1;
                break;
            }
            if(c == 'l' || c == 'L') {
                if(prev_character != 'l' && prev_character != 'L') {
                    toggleLogging( frequencies_to_record );
                }
            }
            else {
                i=0;
            }
        }
 
        //fprintf(stderr,"%d ",i);
    

            std::map<unsigned, std::pair<unsigned, glm::vec2> > positions = locator.loadPosition();
            
            if(positions.size() != 0) {

                unsigned current_iteration_timestamp_peak = 0;
                //
                sendPositions( positions, current_iteration_timestamp_peak );


                if(current_iteration_timestamp_peak > latest_received_timestamp) {
                    latest_received_timestamp = current_iteration_timestamp_peak;
                }
            }



            //if(show_signalvis) {
                draw_signal_plot(signal_plot_window_, locator);
            //}
        
    }
    nonblock(NB_DISABLE);
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
    std::cout << "Doing something\n";
}

void sendPositions( std::map<unsigned, std::pair<unsigned, glm::vec2> > const& located_positions, 
                    unsigned& frontier_timestamp ) {

    for(auto const& frequency_position_entry : located_positions ) {     

        if(latest_received_timestamp < frequency_position_entry.second.first) {
           frontier_timestamp = frequency_position_entry.second.first;


            std::cout << "Frequency: " << frequency_position_entry.first 
                      << "("  << frequency_position_entry.second.second[0] 
                      << ", " << frequency_position_entry.second.second[1]
                      << ")\n";
            if(logging){
                test_logger.update(frequency_position_entry.first, frequency_position_entry.second.second);
            }
        }
    }
}

void toggleLogging( std::vector<unsigned> frequencies_to_log ) {
    //get timestamp
    starttime = time(NULL);
    endtime = starttime +60;
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    std::string timestamp = 
        std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) + "_" + 
        std::to_string((now->tm_hour)%24) + ":" + std::to_string((now->tm_min)%60) + ":" + std::to_string((now->tm_sec)%60);
    if(logging){
        std::cout<<"ended logging of " << timestamp << " by pressing 'l'\n";
        logging = false;
        test_logger.closeFiles();
    } else{
        std::cout<<"started logging of " << timestamp << "\n";
        logging = true;
        std::vector<std::pair<std::string, std::string>> filenames;
        
        for(auto const& freq : frequencies_to_log ){
          filenames.push_back({std::to_string(freq), timestamp});
        }
        test_logger.openFiles(filenames);
    }
}