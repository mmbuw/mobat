#ifndef AIRHOCKEY_H
#define AIRHOCKEY_H


#include "game.hpp"
#include "drawable_object.hpp"
#include "score.hpp"
#include "configurator.hpp"
#include "token_position.hpp"


#include "../udp_receiver/udp_receiver.hpp"


#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>







class Airhockey{
  private:
  	TTT::UdpReceiver receiver_;

  public:
  	Airhockey();
  	void forwardToken(token_position const& in);
  	
  	TTT::Game game_;




};

#endif