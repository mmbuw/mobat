#include <iostream>
#include <vector>
#include "recorder.hpp"

void stream_buffer(std::ostream&, unsigned char[], unsigned size);

int main(int argc, char *argv[])
{
  int err;

  std::vector<std::string> devices{Recorder::get_pcms()};
 
  // std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  Config playback_config{1, 44100, 23219};
  std::vector<std::string> playback_devices{Recorder::get_supporting_devices(devices, playback_config, SND_PCM_STREAM_PLAYBACK)};

  // std::cout << "----playback PCMs----------------------------------------------" << std::endl;
  // for(auto device : playback_devices) {
  //   std::cout << device << std::endl;
  // }
  // open chosen device
  device playback_device{playback_devices[0], SND_PCM_STREAM_PLAYBACK};
  if(!playback_device) return 1;

  playback_config.install(playback_device);

  Recorder recorder{1, 44100, 2000000};
  recorder.record();

  // stream_buffer(std::cout, buffer, sizeof(buffer) / sizeof(*buffer));

  unsigned char* buffer = recorder.buffer(); 
  for(unsigned char* start = &buffer[0]; start < &buffer[recorder.buffer_length()]; start += playback_config.period_bytes()) {
    // std::cout << --loops << std::endl;
    err = snd_pcm_writei(playback_device, start, playback_config.period_frames());
    if(err == -EPIPE) {
      snd_pcm_prepare(playback_device);
    }
    else if(err != playback_config.period_frames()) {
      std::cerr << "write to audio interface failed " << snd_strerror(err) << std::endl;
      return 1;
    }
  } 

  snd_pcm_drain(playback_device);

  return 0;
}

void stream_buffer(std::ostream& os, unsigned char buffer[], unsigned size) {
  for(unsigned i = 0; i < size; ++i) {
    os << buffer[i];
  }
}