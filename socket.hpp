#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstdint>

#if defined(_WIN32)
  #define PLATFORM_WINDOWS
#else
  #define PLATFORM_UNIX
#endif

#ifdef PLATFORM_WINDOWS
  #include <winsock2.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif

#ifdef PLATFORM_WINDOWS
  #pragma comment( lib, "wsock32.lib" )
#endif

#ifdef PLATFORM_WINDOWS
  typedef int socklen_t;
#endif

class Socket {
 public:
  Socket();
  Socket(unsigned short port, bool non_blocking = true);
  Socket(Socket const&) = delete;
  Socket(Socket&&);
  ~Socket();

  Socket& operator=(Socket const&) = delete;
  Socket& operator=(Socket&&);

  void send(sockaddr_in const& address, uint8_t* packet_data, int packet_size);

  std::size_t recieve(sockaddr_in const& source_address, std::uint8_t* packet_data, std::size_t packet_size);
 
 private:
  void make_nonblocking();
 
  unsigned short port_;
  int handle_;
  bool blocking_;

  static std::size_t num_sockets;

  friend void swap(Socket&, Socket&);
};


#endif