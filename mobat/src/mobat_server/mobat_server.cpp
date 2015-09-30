#include "mobat_server.hpp"

MobatServer::MobatServer ( std::array<std::uint8_t, 4> receiver_address, std::uint16_t receiver_port, std::uint16_t sender_port ) 
	: sender_socket_(sender_port),
	  target_address_{receiver_address[0], 
	  				 receiver_address[1],
	  				 receiver_address[2],
	  				 receiver_address[3],
	  				 receiver_port},
	  locator_{4},
	  position_logger_(),
	  is_logging_{false},
	  frontier_timestamp_{0},
	  starttime_{0},
	  endtime_{starttime_+60} {

}

MobatServer::~MobatServer() {
}

void MobatServer::run() {


  std::vector<unsigned> frequencies_to_record{configurator().getList("known_frequencies")};

  locator_.setFrequenciesToRecord(frequencies_to_record);

  auto recording_thread = std::thread(&Locator::recordPosition, &locator_);

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

  char c = '0';
  char prev_character = '0';
  int i=0;

  nonblock(NB_ENABLE);
  while( true ) {
    usleep(1);
    i=kbhit();

    if (i!=0){
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
      } else {
        i=0;
      }
    }

    std::map<unsigned, std::pair<unsigned, glm::vec2> > positions = locator_.loadPosition();
        
    if(positions.size() != 0) {

      unsigned current_iteration_timestamp_peak = 0;
      //
      sendPositions( positions, current_iteration_timestamp_peak );

      if(current_iteration_timestamp_peak > frontier_timestamp_) {
        frontier_timestamp_ = current_iteration_timestamp_peak;
      }
    }

    //if(show_signalvis) {
      drawSignalPlot(signal_plot_window_, locator_);
    //}
    
}

nonblock(NB_DISABLE);
locator_.shutdown();
recording_thread.join();

return;
}

int MobatServer::kbhit() {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

void MobatServer::nonblock( int state ) {
  struct termios ttystate;

  //get the terminal state
  tcgetattr(STDIN_FILENO, &ttystate);

  if (state==NB_ENABLE) {
    //turn off canonical mode
    ttystate.c_lflag &= ~ICANON;
    //minimum of number input read.
    ttystate.c_cc[VMIN] = 1;
  }
  else if (state==NB_DISABLE) {
    //turn on canonical mode
    ttystate.c_lflag |= ICANON;
  }
  //set the terminal attributes.
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void MobatServer::sendPositions( std::map<unsigned, std::pair<unsigned, glm::vec2> > const& located_positions, 
                    			 unsigned int& frontier_timestamp) {

    for(auto const& frequency_position_entry : located_positions ) {     

        if(frontier_timestamp_ < frequency_position_entry.second.first) {
           frontier_timestamp = frequency_position_entry.second.first;


            std::cout << "Frequency: " << frequency_position_entry.first 
                      << "("  << frequency_position_entry.second.second[0] 
                      << ", " << frequency_position_entry.second.second[1]
                      << ")\n";
			

			token_position token_packet{frequency_position_entry.first, 
								        frequency_position_entry.second.second[0] , 
								        frequency_position_entry.second.second[1], 
								        frontier_timestamp};
			
			packet::send<token_position>(sender_socket_, target_address_, token_packet);

            if(is_logging_){
                position_logger_.update(frequency_position_entry.first, frequency_position_entry.second.second);
            }
        }
    }
}

void MobatServer::toggleLogging( std::vector<unsigned> const& frequencies_to_log ) {
	//get timestamp
    starttime_ = time(NULL);
    endtime_ = starttime_ + 60;
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    std::string timestamp = 
        std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) + "_" + 
        std::to_string((now->tm_hour)%24) + ":" + std::to_string((now->tm_min)%60) + ":" + std::to_string((now->tm_sec)%60);
    if(is_logging_){
        std::cout<<"ended logging of " << timestamp << " by pressing 'l'\n";
        is_logging_ = false;
        position_logger_.closeFiles();
    } else{
        std::cout<<"started logging of " << timestamp << "\n";
        is_logging_ = true;
        std::vector<std::pair<std::string, std::string>> filenames;
        
        for(auto const& freq : frequencies_to_log ){
          filenames.push_back({std::to_string(freq), timestamp});
        }
        position_logger_.openFiles(filenames);
    }
}

void MobatServer::drawSignalPlot( sf::RenderWindow& window, Locator const& locator) {

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