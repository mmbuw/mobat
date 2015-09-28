#include <iostream>
#include <cstdint>
#include <vector>
#include "socket.hpp"
#include "address.hpp"

int main(int argc, char *argv[]) {

  if(argc > 1) {
    std::cout << "sender" << std::endl;
    Socket socket{6666};
    
    address_ip4 address {127, 0, 0, 1, 30000};

    std::vector<std::uint8_t> data(4);
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
      address_ip4 source_address {127, 0, 0, 1, 6666};

      ssize_t max_packet_size = 4;
      std::vector<std::uint8_t> packet_data(max_packet_size);

      std::size_t bytes = socket.recieve(source_address, packet_data.data(), max_packet_size);

      if (bytes > 0) {
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