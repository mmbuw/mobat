#include <iostream>
#include <cstdint>
#include <vector>
#include "socket.hpp"
#include "address.hpp"

int main(int argc, char *argv[]) {

  if(argc > 1) {
    std::cout << "sender" << std::endl;
    Socket socket{6666};
    
    Address_ip4 target_address{127, 0, 0, 1, 30000};

    std::vector<std::uint8_t> data(4);
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;

    int i = 3;
    while (i > 0) {
      socket.send(target_address, &data[0], data.size());
      --i;
    }
  }
  else {
    std::cout << "reciever" << std::endl;
    Socket socket{30000};
   
    while (true){

      ssize_t max_packet_size = 4;
      std::vector<std::uint8_t> packet_data(max_packet_size);
      
      Address_ip4 source_address{};
      std::size_t bytes = socket.recieve(&source_address, packet_data.data(), max_packet_size);

      if (bytes > 0) {
        std::uint32_t source_ip = source_address.address();
        std::uint16_t source_port = source_address.port();
        std::cout << "recieved from " << source_ip << " from port " << source_port << std::endl;
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