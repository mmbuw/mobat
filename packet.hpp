#ifndef PACKET_HPP
#define PACKET_HPP

#include "socket.hpp"
#include <stdexcept>
#include <typeinfo>

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
// make header depending on hash of contained struct
template<typename T>
const std::uint32_t packet<T>::correct_header = std::uint32_t(typeid(T).hash_code());
template<typename T>
const std::uint8_t packet<T>::correct_footer = 116;

template<typename T>
void send(Socket const& socket, Address const& address, T const& value) {
  packet<T> packet{value};
  socket.send(address, (uint8_t*)&packet, sizeof(packet));
}

template<typename T>
bool receive(Socket const& socket, Address* received_address, T* received_pos) {
  packet<T> received_packet{};
  ssize_t received_bytes = socket.receive(received_address, (uint8_t*)&received_packet, sizeof(received_packet));

  if (received_bytes != sizeof(received_packet)) {
    return false;
  }

  if (received_packet.header != packet<T>::correct_header) {
    return false;
  }

  if (received_packet.footer != packet<T>::correct_footer) {
    throw std::runtime_error("MoBat packet corrupted");
    return false;
  }

  *received_pos = received_packet.value;
  return true;
}
#endif