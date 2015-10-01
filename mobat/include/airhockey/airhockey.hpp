#ifndef AIRHOCKEY_H
#define AIRHOCKEY_H


#include "game.hpp"
#include "drawable_object.hpp"
#include "score.hpp"
#include "configurator.hpp"
#include "token_packet.hpp"


#include "receiver.hpp"
#include "socket.hpp"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>







class Airhockey{
  private:


  public:
  	Airhockey();
  	Airhockey(std::uint16_t port);
  	void forwardToken(token_packet const& in);
  	
  	Receiver<token_packet> receiver_;
  	TTT::Game game_;




};

#endif