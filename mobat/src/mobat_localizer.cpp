#include "configurator.hpp"
#include "mobat_server.hpp"

int main(int argc, char** argv) {

  std::string file_name{"default_server_config.conf"};
  if(argc > 1) {
    file_name = argv[1];
  }

  // load the server configurations from the provided file
  configurator().read(file_name);
  
  // create the server and run it's main loop
  // note, that the server implementation is blocking
  MobatServer sending_server;
  sending_server.run();

  return 0;
}

