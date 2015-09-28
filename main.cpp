#include <iostream>
#include <cstdint>
#include <vector>
#include "socket.hpp"

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
    Socket socket{6666};
    
    sockaddr_in address = make_address(127, 0, 0, 1, 30000);

    std::vector<uint8_t> data(4);
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;

    int i = 3;
    while (i > 0) {
      socket.send(address, &data[0], data.size());
      --i;
    }
  }
  else {
    std::cout << "reciever" << std::endl;
    Socket socket{30000};
   
    while (true){
      sockaddr_in source_address = make_address(127, 0, 0, 1, 6666);

      size_t max_packet_size = 4;
      // uint8_t* packet_data;
      std::vector<uint8_t> packet_data(max_packet_size);
      size_t packet_size = max_packet_size;

      std::size_t bytes = socket.recieve(source_address, packet_data.data(), packet_size);

      if (bytes > 0) {
        unsigned int from_address = ntohl(source_address.sin_addr.s_addr);
        unsigned int from_port = ntohs(source_address.sin_port);

        std::cout << "recieved from " << from_address << " port " << from_port << std::endl;

        // process received packet
        for (auto const& a : packet_data) {
          std::cout << a << ", ";
        }
        std::cout << std::endl;
        return 0;
      }
    }

  }
  return 0;
}