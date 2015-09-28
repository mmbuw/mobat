#include "socket.hpp"

#include "address.hpp"

#include <stdexcept>

#ifdef PLATFORM_UNIX
  #include <netinet/in.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif

#ifdef PLATFORM_WINDOWS
  #pragma comment( lib, "wsock32.lib" )
#endif

std::size_t Socket::num_sockets = 0;

Socket::Socket()
 :port_{0}
 ,handle_{-1}
 ,blocking_{true}
 {}

Socket::Socket(unsigned short port, bool non_blocking)
 :port_{port}
 ,handle_{-1}
 ,blocking_{!non_blocking}
{
  #ifdef PLATFORM_WINDOWS
    if(num_sockets == 0) {
      WSADATA WsaData;
      std::errc error = WSAStartup(MAKEWORD(2,2), &WsaData);
      if (error != NO_ERROR) {
        throw std::runtime_error("Could not start socket control");
      }      
    }
  #endif

  handle_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (handle_ <= 0) {
    throw std::runtime_error("Failed to create socket");
  }
  
  #ifdef PLATFORM_WINDOWS
  ++num_sockets;
  #endif
  // create address at port
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  // bind socket to port
  int error = bind(handle_, (const sockaddr*)&address, sizeof(sockaddr_in));
  if (error < 0) {
    throw std::runtime_error("Failed to bind socket to port " + std::to_string(port));
  }

  if (!blocking_) {
    make_nonblocking();
  }
}

Socket::Socket(Socket&& socket)
 :Socket{}
{
  swap(*this, socket);
}

Socket::~Socket() {
  #ifdef PLATFORM_UNIX
    close(handle_);
  #elif PLATFORM_WINDOWS
    closehandle(handle_);
  #endif

  #ifdef PLATFORM_WINDOWS
    --num_sockets;
    // on windows do cleanup
    if(num_sockets == 0) {
        WSACleanup();
    }
  #endif
}

Socket& Socket::operator=(Socket&& socket) {
  swap(*this, socket);
  return *this;
}


void Socket::make_nonblocking() {
  #ifdef PLATFORM_UNIX
    int nonBlocking = 1;
    if (fcntl(handle_, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
      throw std::runtime_error("failed to set non-blocking\n");
    }
  #elif PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
    if (ioctlsocket(handle_, FIONBIO, &nonBlocking) != 0) {
      throw std::runtime_error("failed to set non-blocking\n");
    }
  #endif
}

void Socket::send(address const& target_address, std::uint8_t* packet_data, ssize_t packet_size) {
  ssize_t sent_bytes = sendto(handle_, packet_data, packet_size, 0, target_address.base_address(), target_address.bytes());

  if (sent_bytes != packet_size) {
    printf("failed to send packet\n");
    throw std::runtime_error("Could only send " 
      + std::to_string(sent_bytes) + " instead of "
      + std::to_string(packet_size) + " bytes");
  }
}

std::size_t Socket::recieve(address const& source_address, std::uint8_t* packet_data, ssize_t packet_size) {
  sockaddr base_address = *source_address.base_address(); 
  socklen_t address_bytes = source_address.bytes();
  ssize_t bytes = recvfrom(handle_, 
                   packet_data, 
                   packet_size,
                   0, 
                   &base_address, 
                   &address_bytes);
  if (bytes > 0) {
    // if the expected address type is smaller than the recieved one
    if (address_bytes > source_address.bytes()) {
      throw std::overflow_error("Address type of " + std::to_string(source_address.bytes()) 
      + " bytes does not match source address of " + std::to_string(address_bytes));
    }
    return bytes;
  }
  return 0;
}

void swap(Socket& a, Socket& b) {
  std::swap(a.port_, b.port_);
  std::swap(a.handle_, b.handle_);
  std::swap(a.handle_, b.handle_);
  std::swap(a.blocking_, b.blocking_);
}