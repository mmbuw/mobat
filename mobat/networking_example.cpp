#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>
#include "address.hpp"
#include "packet.hpp"
#include "token_packet.hpp"

int main(int argc, char *argv[]) {

  if(argc > 1) {
    std::cout << "sender" << std::endl;
    Socket socket{6666};
    Address_ip4 target_address{127, 0, 0, 1, 30000};

    token_packet test{1, 1337.0f, 1138.0f};
    packet::send<token_packet>(socket, target_address, test);
  }
  else {
    std::cout << "receiver" << std::endl;
    Socket socket{30000};
   
    while (true){
      Address_ip4 source_address{};
      
      token_packet received_pos{};
      bool got_package = packet::receive<token_packet>(socket, &source_address, &received_pos);
      if (got_package) {
        std::cout << "received from ip " << source_address.address() << " from port " << source_address.port() << std::endl;
        std::cout << "id " << received_pos.id << ", pos (" << received_pos.x << ", " << received_pos.y << ")" << std::endl;
        break;
      }
    }
  }
  return 0;
}