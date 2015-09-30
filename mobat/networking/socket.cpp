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
    int error = fcntl(handle_, F_SETFL, O_NONBLOCK, nonBlocking);
    if (error == -1) {
      throw std::runtime_error("Failed to set socket non-blocking");
    }
  #elif PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
    int error = ioctlsocket(handle_, FIONBIO, &nonBlocking);
    if (error != 0) {
      throw std::runtime_error("Failed to set socket non-blocking");
    }
  #endif
}

void Socket::send(Address const& target_address, std::uint8_t* packet_data, ssize_t packet_bytes) const {
  ssize_t sent_bytes = sendto(handle_,
                              packet_data,
                              packet_bytes,
                              0,
                              target_address.base_address(),
                              target_address.bytes());

  if (sent_bytes != packet_bytes) {
    printf("failed to send packet\n");
    throw std::runtime_error("Could only send " 
      + std::to_string(sent_bytes) + " instead of "
      + std::to_string(packet_bytes) + " bytes");
  }
}

ssize_t Socket::receive(Address* source_address, std::uint8_t* buffer_ptr, ssize_t buffer_bytes) const {
  // dont crash if source address is nullpter
  socklen_t address_bytes = 0;
  sockaddr* base_address = NULL;
  if (source_address != nullptr) {
   address_bytes = source_address->bytes();
   base_address = source_address->base_address();
  }

  ssize_t received_bytes = recvfrom(handle_, 
                   buffer_ptr, 
                   buffer_bytes,
                   MSG_TRUNC,     //return untruncated packet length
                   base_address, 
                   &address_bytes);

  if (received_bytes > 0) {
    // if the expected address type is smaller than the received one
    if (source_address != nullptr) {
      if (address_bytes > source_address->bytes()) {
        throw std::overflow_error("Address type of " + std::to_string(source_address->bytes()) 
        + " bytes does not match source address of " + std::to_string(address_bytes));
      }
    }
    // received packet bigger than buffer
    if (received_bytes > buffer_bytes) {
      throw std::overflow_error("Packet of " + std::to_string(received_bytes) 
      + " bytes larger than buffer of " + std::to_string(buffer_bytes) + " bytes");
    }
    return received_bytes;
  }
  return 0;
}

void swap(Socket& a, Socket& b) {
  std::swap(a.port_, b.port_);
  std::swap(a.handle_, b.handle_);
  std::swap(a.handle_, b.handle_);
  std::swap(a.blocking_, b.blocking_);
}