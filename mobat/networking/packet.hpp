#ifndef PACKET_HPP
#define PACKET_HPP

#include "socket.hpp"
#include <stdexcept>
#include <typeinfo>

namespace packet {
// struct to hold object of type T with header for identification
// and footer for integrity check
template<typename T>
struct packet_t {
  packet_t()
   :value()
  {}

  packet_t(T val)
   :value(val)
  {}

  T value;
};

// send a packet containing object to type T from socket to address
template<typename T>
void send(Socket const& socket, Address const& address, T const& value) {
  packet_t<T> packet{value};
  socket.send(address, (uint8_t*)&packet, sizeof(packet));
}

// try to receive a packet containing object to type T
// write out address and value and return true if successfull 
template<typename T>
bool receive(Socket const& socket, Address* received_address, T* received_val) {
  packet_t<T> received_packet{};
  ssize_t received_bytes = socket.receive(received_address, (uint8_t*)&received_packet, sizeof(received_packet));

  if (received_bytes != sizeof(received_packet)) {
    return false;
  }

  *received_val = received_packet.value;
  return true;
}

}

#endif