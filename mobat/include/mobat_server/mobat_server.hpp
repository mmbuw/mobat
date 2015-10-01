#ifndef MOBAT_SERVER_H
#define MOBAT_SERVER_H

#include "locator.hpp"
#include "configurator.hpp"
#include "test.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>

#include <time.h>
#include <boost/filesystem.hpp>

#include <termios.h>


#include "address.hpp"
#include "packet.hpp"
#include "token_packet.hpp"

#define NB_ENABLE 1
#define NB_DISABLE 0


class MobatServer {

public:
  MobatServer( std::array<uint8_t, 4> receiver_address = {127,0,0,1}, std::uint16_t receiver_port = 30000, std::uint16_t sender_port = 6666  );
  ~MobatServer();

  void run();
  //toggleLogging();
private:
  // receiver for located positions
  Socket sender_socket_;
  Address_ip4 target_address_;

  Locator locator_;

  // position logger
  TTT::Test position_logger_;
  // is logging currently enabled
  bool is_logging_;
  // last received timestamp
  unsigned frontier_timestamp_;

  //logging start and endtimes
  time_t starttime_;// = time(0);
  time_t endtime_;// = starttime + 60;

  bool is_verbose_;
  /*
    functions "kbhit" and "nonblock" taken from: 
    http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
    (Accessed: 30th September 2015)
  */
  int kbhit();

  void nonblock(int state);


  void sendPositions( std::map<unsigned, std::pair<unsigned, glm::vec2> > const& located_positions, 
                      unsigned& frontier_timestamp );

  void toggleLogging( std::vector<unsigned> const& frequencies_to_log );

  void drawSignalPlot( sf::RenderWindow& window, Locator const& locator);

};

#endif //MOBAT_SERVER_H