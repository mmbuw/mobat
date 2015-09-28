#include <iostream>
#include <cstdint>
#include <vector>

// platform detection
#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
  #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define PLATFORM PLATFORM_MAC
#else
  #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
  #include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
  #pragma comment( lib, "wsock32.lib" )
#endif

#if PLATFORM == PLATFORM_WINDOWS
  typedef int socklen_t;
#endif

bool initialize_sockets() {
  #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
  #else
    return true;
  #endif
}

void shutdown_sockets() {
  #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
  #endif
}

void close_socket(int handle) {
  #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close(handle);
  #elif PLATFORM == PLATFORM_WINDOWS
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
  #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    int nonBlocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
      printf("failed to set non-blocking\n");
      return false;
    }
  #elif PLATFORM == PLATFORM_WINDOWS
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
    
  std::cout << "hello" << std::endl;

  bool success = initialize_sockets();
  if (!success) {
    return 1;
  }
  int handle = 0;

  make_nonblocking(handle);

  if(argc > 1) {
    handle = create_socket(6666);
    std::cout << handle << std::endl;
    std::cout << "sender" << std::endl;
    sockaddr_in address = make_address(127, 0, 0, 1, 30000);

    std::vector<uint8_t> data(4);
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;

    int i = 3;
    while(i > 0) {
      send_packet(handle, address, &data[0], data.size());
      --i;
    }
  }
  else {
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
                           (socklen_t*)&from_length);

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
  }
  close_socket(handle);

  shutdown_sockets();
  return 0;
}