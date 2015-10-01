#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>
#include "address.hpp"
#include "packet.hpp"
#include "receiver.hpp"
#include "token_packet.hpp"

int i = 0;
void rec_func(token_packet const& received_pos) {
  std::cout << "id " << received_pos.id << ", pos (" << received_pos.x << ", " << received_pos.y << ")" << std::endl;  
  ++i;
} 

class Test {
 public:
  void rec_func(token_packet const& received_pos) {
  // void rec_func() {
    std::cout << "id " << received_pos.id << ", pos (" << received_pos.x << ", " << received_pos.y << ")" << std::endl;  
    ++i;
  } 

  int i = 0;
};

int main(int argc, char *argv[]) {

  if (argc > 1) {
    std::cout << "sender" << std::endl;
    Socket socket{6666};
    Address_ip4 target_address{127, 0, 0, 1, 30000};

    token_packet test{1, 1337.0f, 1138.0f, 0};
    packet::send<token_packet>(socket, target_address, test);
    packet::send<token_packet>(socket, target_address, test);
    packet::send<token_packet>(socket, target_address, test);
  }
  else {
    std::cout << "receiver" << std::endl;
    Test test;

    Receiver<token_packet> rec{30000};
    rec.setReceiveCallback(&Test::rec_func, test);
    rec.startListening();

    while (test.i < 3) {

    }
  }
  return 0;
}