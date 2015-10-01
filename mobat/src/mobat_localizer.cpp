#include "configurator.hpp"
#include "mobat_server.hpp"

void tryParsingAddress(Address_ip4& in_address, std::uint16_t& in_sender_port) {
  
  std::uint32_t receiver_ip(0x00);
  std::uint16_t receiver_port(0x00);
  std::uint16_t sender_port(0x00);

  try{ 
    unsigned char address_component = 0x00;
    for(int comp_idx = 3; comp_idx >= 0; --comp_idx) {
      switch(comp_idx) {
        case 0:
          address_component = (unsigned char) (configurator().getUint("receiver_addr_p3"));
          break;
        case 1:
          address_component = (unsigned char) (configurator().getUint("receiver_addr_p2"));
          break;
        case 2:
          address_component = (unsigned char) (configurator().getUint("receiver_addr_p1"));
          break;
        case 3:
          address_component = (unsigned char) (configurator().getUint("receiver_addr_p0"));
          break;
        default:
            throw;
      }

      receiver_ip |= (address_component << ( comp_idx * 8 ));
    }
  } catch (...){
    std::cout << "Receiver address parsing exception occured. Using 127.0.0.1 instead.\n";
    receiver_ip =   (127 << 24) 
                  | (  0 << 16) 
                  | (  0 <<  8)
                  | (  1 <<  0); 
  }

  try{ 
    receiver_port = (configurator().getUint("receiver_port"));
  } catch (...) {
    std::cout << "Receiver port parsing exception occured. Using 30000 instead.\n";
    receiver_port = 30000;
  }

  try{ 
    sender_port = (configurator().getUint("sender_port"));
  } catch (...) {
    std::cout << "Sender port parsing exception occured. Using 6666 instead.\n";
    sender_port = 6666;
  }

  in_address.set_address(receiver_ip);
  in_address.set_port(receiver_port);

  in_sender_port = sender_port;
}


int main(int argc, char** argv) {

  std::string file_name{"default_server_config.conf"};
  if(argc > 1) {
    file_name = argv[1];
  }

  // load the server configurations from the provided file
  configurator().read(file_name);
  
  Address_ip4 parsed_target_address{0x00, 0x00, 0x00, 0x00, 0x00};
  std::uint16_t parsed_sender_port = 0x00;
  tryParsingAddress(parsed_target_address, parsed_sender_port);

  // create the server and run it's main loop
  // note, that the server implementation is blocking
  MobatServer sending_server(parsed_target_address, parsed_sender_port);
  sending_server.run();

  return 0;
}

