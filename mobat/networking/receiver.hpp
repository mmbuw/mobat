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
  	stop_listening();
  }

  void set_receive_callback(std::function<void(T const&)> const& func) {
    callback_ = func;
  }
  
  template<typename S>
  void set_receive_callback(void (S::*func)(T const&), S& object) {
    callback_ = std::bind(func, &object);
  }

  void start_listening() {
    listen_ = true;
  	listen_thread_ = std::thread{&Receiver::listen, this};
  }

  void stop_listening() {
    listen_ = false;
    listen_thread_.join();
  }

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
  std::function<void(T const&)> callback_;
  std::thread listen_thread_;
};

#endif