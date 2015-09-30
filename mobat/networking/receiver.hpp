#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "packet.hpp"

#include <functional>

template<typename T>
class Receiver {
 public:
  Receiver(std::uint16_t port) 
   :socket_{port}
   ,listen_{false}
  {}

  void set_receive_callback(std::function<void(T)> const& func) {
    callback_ = func;
  }

  void listen() {
    std::cout << "receiver" << std::endl;
    listen_ = true;

    while (listen_){
      Address_ip4 source_address{};
      
      T received_data{};
      bool got_package = packet::receive<T>(socket_, &source_address, &received_data);
      if (got_package) {
        callback_(received_data);
      }
    }
  }

  void stop_listening() {
    listen = false;
  }

 private:
  Socket socket_;
  bool listen_;
  std::function<void(T)> callback_;
};

#endif