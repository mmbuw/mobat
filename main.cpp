#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>
#include "socket.hpp"
#include "address.hpp"
#include "packet.hpp"

int main(int argc, char *argv[]) {

  if(argc > 1) {
    std::cout << "sender" << std::endl;
    Socket socket{6666};
    Address_ip4 target_address{127, 0, 0, 1, 30000};

    position_t test{1, 1337.0f, 1138.0f};
    send<position_t>(socket, target_address, test);
  }
  else {
    std::cout << "receiver" << std::endl;
    Socket socket{30000};
   
    while (true){
      ssize_t max_packet_size = 5;
      std::vector<std::uint8_t> packet_data(max_packet_size);
      
      Address_ip4 source_address{};
      
      position_t received_pos{};
      bool got_package = receive<position_t>(socket, &source_address, &received_pos);
      if (got_package) {
        std::cout << "received from ip " << source_address.address() << " from port " << source_address.port() << std::endl;
        std::cout << "id " << received_pos.id << ", pos (" << received_pos.x << ", " << received_pos.y << ")" << std::endl;
        break;
      }
    }
  }
  return 0;
}