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
  {}

  ~Receiver() {
  	if(listen_) {
  		stopListening();
  	}
  }

  void setReceiveCallback(std::function<void(T const&)> const& func) {
    callback_ = func;
  }
  
  template<typename S>
  void setReceiveCallback(void (S::*func)(T const&), S& object) {
    callback_ = std::bind(func, &object);
  }

  void startListening() {
    listen_ = true;
  	listen_thread_ = std::thread{&Receiver::listen, this};
  }

  void stopListening() {
    listen_ = false;
    listen_thread_.join();
  }

  std::function<void(T const&)> callback_;
 private:
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
};

#endif