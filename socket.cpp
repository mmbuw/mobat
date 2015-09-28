#include "socket.hpp"

#include <stdexcept>

std::size_t Socket::num_sockets = 0;

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

void Socket::send(sockaddr_in const& address, uint8_t* packet_data, int packet_size) {
  int sent_bytes = sendto(handle_, (const char*)packet_data, packet_size, 0, 
      (sockaddr*)&address, 
      sizeof(sockaddr_in));

  if (sent_bytes != packet_size) {
    printf("failed to send packet\n");
    throw std::runtime_error("Could only send " 
      + std::to_string(sent_bytes) + " instead of "
      + std::to_string(packet_size) + " bytes");
  }
}

std::size_t Socket::recieve(sockaddr_in const& source_address, std::uint8_t* packet_data, std::size_t packet_size) {
  socklen_t from_length = sizeof(source_address);
  ssize_t bytes = recvfrom(handle_, 
                   packet_data, 
                   packet_size,
                   0, 
                   (sockaddr*)&source_address, 
                   &from_length);
  if(bytes > 0) {
    return bytes;
  }
  return 0;
}