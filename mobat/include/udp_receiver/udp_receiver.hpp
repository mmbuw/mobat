#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H


//#include "../locator/locator.hpp"
// #include "../visualization/table_visualizer.hpp"
// #include "../visualization/score.hpp"
// #include "../visualization/configurator.hpp"


#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <thread>
#include <map>
#include <set>
#include <chrono>

#include "token.hpp"




namespace MoB {

	class UdpReceiver {

		private:
			std::map<unsigned, Token> positions_;
			std::map<unsigned, Token> recognized_tokens_;
			std::chrono::high_resolution_clock::time_point last_time_stamp_;
			
		public:
			UdpReceiver();
			~UdpReceiver();
			//recieves a pair of id and position with which it updates positions_
			void receive(std::pair<unsigned, glm::vec2> const& entry);
			//prints all stored positions
			void printAll() const;
			//prints position of requested ID, throws "out of range" exception if ID does not exist in positions_
			void print(unsigned const& ID) const;
			//returns position of requested ID, throws "out of range" exception if ID does not exist in positions_
			glm::vec2 positionOf(unsigned const& ID) const;
			//draws all IDs
			void draw(sf::RenderWindow& canvas);
			//directionFromTo
			glm::vec2 directionFromTo(unsigned ID1, unsigned ID2) const;
			//removes lost tokens
			void updateTokens();
	};

}; //end MoB
#endif






