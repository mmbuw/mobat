#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstdint>
#if defined(_WIN32)
  #define PLATFORM_WINDOWS
  #include <winsock2.h>
#else
  #define PLATFORM_UNIX
  #include <sys/socket.h>
#endif

class Address;

class Socket {
 public:
  Socket();
  Socket(unsigned short port, bool non_blocking = true);
  Socket(Socket const&) = delete;
  Socket(Socket&&);
  ~Socket();

  Socket& operator=(Socket const&) = delete;
  Socket& operator=(Socket&&);

  void send(Address const& target_address, std::uint8_t* packet_data, ssize_t packet_bytes);

  std::size_t recieve(Address* source_address, std::uint8_t* buffer_data, ssize_t buffer_size);
 
 private:
  void make_nonblocking();
 
  unsigned short port_;
  int handle_;
  bool blocking_;

  static std::size_t num_sockets;

  friend void swap(Socket&, Socket&);
};


#endif