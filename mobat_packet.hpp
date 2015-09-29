#ifndef MOBAT_PACKET_HPP
#define MOBAT_PACKET_HPP

#include "socket.hpp"

struct position_t {
  position_t();
  position_t(std::uint8_t i, float xx, float yy);

  std::uint8_t id;
  float x;
  float y;
};

struct mobat_packet {
  mobat_packet();
  mobat_packet(std::uint8_t i, float x, float y);
  mobat_packet(position_t pos);

  const std::uint32_t header = correct_header;
  position_t position;
  const std::uint8_t footer = correct_footer; 

  static const std::uint32_t correct_header;
  static const std::uint8_t correct_footer;
};

void send_position(Socket const& socket, Address const& address, position_t const& pos);
void send_position(Socket const& socket, Address const& address, std::uint8_t id, float x, float y);

bool receive_position(Socket const& socket, Address* received_address, position_t* received_pos);

#endif