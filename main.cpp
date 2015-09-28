#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>

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

struct socket_t {
  socket_t(unsigned short port, bool non_blocking = true)
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

  ~socket_t() {
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

  void make_nonblocking() {
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

  void send_packet(sockaddr_in const& address, uint8_t* packet_data, int packet_size) {
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

  unsigned short port_;
  int handle_;
  bool blocking_;

  static std::size_t num_sockets;
};

std::size_t socket_t::num_sockets = 0;

bool initialize_sockets() {
  #ifdef PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
  #else
    return true;
  #endif
}

void shutdown_sockets() {
  #ifdef PLATFORM_WINDOWS
    WSACleanup();
  #endif
}

void close_socket(int handle) {
  #ifdef PLATFORM_UNIX
    close(handle);
  #elif PLATFORM_WINDOWS
    closehandle(socket);
  #endif
}

int create_socket(unsigned short port) {
  int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (handle <= 0) {
    printf( "failed to create socket\n" );
    return -1;
  }
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons((unsigned short)port);

  if (bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0) {
    printf( "failed to bind socket\n" );
    return -1;
  }
  return handle;
}

bool make_nonblocking(int handle) {
  #ifdef PLATFORM_UNIX
    int nonBlocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
      printf("failed to set non-blocking\n");
      return false;
    }
  #elif PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
    if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {
      printf("failed to set non-blocking\n");
      return false;
    }
  #endif
  return true;
}

bool send_packet(int handle, sockaddr_in const& address, uint8_t* packet_data, int packet_size) {
  int sent_bytes = sendto(handle, (const char*)packet_data, packet_size, 0, 
      (sockaddr*)&address, 
      sizeof(sockaddr_in));

  if (sent_bytes != packet_size) {
    printf("failed to send packet\n");
    std::cout << "sent " << sent_bytes << " instead of " << packet_size << " bytes" << std::endl;
    return false;
  }

  return true;
}

sockaddr_in make_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port) {
  unsigned int address = ( a << 24 ) | 
                         ( b << 16 ) | 
                         ( c << 8  ) | 
                           d;

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(address);
  addr.sin_port = htons(port);
  return addr;
}

int main(int argc, char *argv[]) {

  if(argc > 1) {
    std::cout << "sender" << std::endl;
    socket_t socket{6666};
    
    sockaddr_in address = make_address(127, 0, 0, 1, 30000);

    std::vector<uint8_t> data(4);
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;

    int i = 3;
    while(i > 0) {
      socket.send_packet(address, &data[0], data.size());
      --i;
    }
  }
  else {
    bool success = initialize_sockets();
    if (!success) {
      return 1;
    }
    int handle = 0;

    make_nonblocking(handle);
    std::cout << "reciever" << std::endl;
    while (true){
      handle = create_socket(30000);
      std::cout << handle << std::endl;
      // unsigned char packet_data[256];
      // unsigned int max_packet_size = sizeof(packet_data);

      sockaddr_in source_address = make_address(127, 0, 0, 1, 6666);
      // sockaddr_in from;
      socklen_t from_length = sizeof(source_address);

      int max_packet_size = 4;
      // uint8_t* packet_data;
      std::vector<uint8_t> packet_data(max_packet_size);

      ssize_t bytes = recvfrom(handle, 
                           packet_data.data(), 
                           max_packet_size,
                           0, 
                           (sockaddr*)&source_address, 
                           &from_length);

      if (bytes <= 0) {
        // break;
      }
      else {
        unsigned int from_address = ntohl(source_address.sin_addr.s_addr);
        unsigned int from_port = ntohs(source_address.sin_port);

        std::cout << "recieved from " << from_address << " port" << from_port << std::endl;

        // process received packet
        for (auto const& a : packet_data) {
          std::cout << a << ", ";
        }
        std::cout << std::endl;
        return 0;
      }
    }
    close_socket(handle);

    shutdown_sockets();
  }
  return 0;
}