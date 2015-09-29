#include "mobat_packet.hpp"
#include <stdexcept>

position_t::position_t()
 :id{0}
 ,x{0}
 ,y{0}
{}

position_t::position_t(std::uint8_t i, float xx, float yy)
 :id{i}
 ,x{xx}
 ,y{yy}
{}

const std::uint32_t mobat_packet::correct_header = 1631743821; // MoBa in ASCII
const std::uint8_t mobat_packet::correct_footer = 116; // t in ASCII 

mobat_packet::mobat_packet()
 :position()
{}

mobat_packet::mobat_packet(std::uint8_t i, float x, float y)
 :position{i, x, y}
{}

mobat_packet::mobat_packet(position_t pos)
 :position(pos)
{}

void send_position(Socket const& socket, Address const& address, position_t const& pos) {
  mobat_packet packet{pos};
  socket.send(address, (uint8_t*)&packet, sizeof(packet));
}

void send_position(Socket const& socket, Address const& address, std::uint8_t id, float x, float y) {
  send_position(socket, address, position_t{id, x, y});
}

bool receive_position(Socket const& socket, Address* received_address, position_t* received_pos) {
  mobat_packet packet{};
  ssize_t received_bytes = socket.receive(received_address, (uint8_t*)&packet, sizeof(packet));

  if (received_bytes != sizeof(packet)) {
    return false;
  }

  if (packet.header != mobat_packet::correct_header) {
    return false;
  }

  if (packet.footer != mobat_packet::correct_footer) {
    throw std::runtime_error("MoBat packet corrupted");
    return false;
  }

  *received_pos = packet.position;
  return true;
}