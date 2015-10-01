#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "packet.hpp"

#include <functional>
#include <thread>

template<typename T>
class Receiver {
 public:
  Receiver(std::uint16_t port) 
   :socket_{port}
   ,listen_{false}
   ,listen_thread_{}
   ,callback_{}
  {}

  ~Receiver() {
  	if (listen_) {
  		stopListening();
  	}
  }
  // set a global/static function as callback
  void setReceiveCallback(std::function<void(T const&)> const& func) {
    callback_ = func;
  }
  // set member function of instnce of S as callback
  template<typename S>
  void setReceiveCallback(void (S::*func)(T const&), S& object) {
    callback_ = std::bind(func, &object, std::placeholders::_1);
  }
  // start listening thread
  void startListening() {
    listen_ = true;
    listen_thread_ = std::thread{&Receiver::listen, this};
  }

  // start stop thread
  void stopListening() {
    listen_ = false;
    listen_thread_.join();
  }

 private:
  // listening thread, invokes callback wehn package of type T arrives
  void listen() {
    while (listen_) {      
      T received_data{};
      bool got_package = packet::receive<T>(socket_, nullptr, &received_data);
      if (got_package) {
        callback_(received_data);
      }
    }
  }

  Socket socket_;
  bool listen_;
  std::thread listen_thread_;
  std::function<void(T const&)> callback_;
};

#endif