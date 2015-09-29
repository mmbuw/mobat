#ifndef PACKET_HPP
#define PACKET_HPP

#include "socket.hpp"
#include <stdexcept>

struct position_t {
  position_t()
   :id{0}
   ,x{0}
   ,y{0}
  {}

  position_t(std::uint8_t i, float xx, float yy)
   :id{i}
   ,x{xx}
   ,y{yy}
  {}

  std::uint8_t id;
  float x;
  float y;
};

template<typename T>
struct packet {
  packet()
   :value()
  {}

  packet(T val)
   :value(val)
  {}


  const std::uint32_t header = correct_header;
  T value;
  const std::uint8_t footer = correct_footer; 

  static const std::uint32_t correct_header;
  static const std::uint8_t correct_footer;
};

template<typename T>
const std::uint32_t packet<T>::correct_header = 1631743821; // MoBa in ASCII
template<typename T>
const std::uint8_t packet<T>::correct_footer = 116; // t in ASCII

template<typename T>
void send(Socket const& socket, Address const& address, T const& value) {
  packet<T> packet{value};
  socket.send(address, (uint8_t*)&packet, sizeof(packet));
}

template<typename T>
bool receive(Socket const& socket, Address* received_address, T* received_pos) {
  packet<T> packet{};
  ssize_t received_bytes = socket.receive(received_address, (uint8_t*)&packet, sizeof(packet));

  if (received_bytes != sizeof(packet)) {
    return false;
  }

  if (packet.header != packet<T>::correct_header) {
    return false;
  }

  if (packet.footer != packet<T>::correct_footer) {
    throw std::runtime_error("MoBat packet corrupted");
    return false;
  }

  *received_pos = packet.value;
  return true;
}
#endif