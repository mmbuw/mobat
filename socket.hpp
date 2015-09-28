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

struct address;

class Socket {
 public:
  Socket();
  Socket(unsigned short port, bool non_blocking = true);
  Socket(Socket const&) = delete;
  Socket(Socket&&);
  ~Socket();

  Socket& operator=(Socket const&) = delete;
  Socket& operator=(Socket&&);

  void send(address const&, std::uint8_t* packet_data, ssize_t packet_size);

  std::size_t recieve(address const&, std::uint8_t* packet_data, ssize_t packet_size);
 
 private:
  void make_nonblocking();
 
  unsigned short port_;
  int handle_;
  bool blocking_;

  static std::size_t num_sockets;

  friend void swap(Socket&, Socket&);
};


#endif