#include <iostream>
#include <vector>
#include "recorder.hpp"

snd_pcm_t* open_device(std::string const&, snd_pcm_stream_t);
void stream_buffer(std::ostream&, unsigned char[], unsigned size);

int main(int argc, char *argv[])
{
  Recorder recorder{1, 44100, 2000000};
  recorder.record();

  std::vector<std::string> devices{Recorder::get_pcms()};
 
  // std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  Config playback_config{1, 44100, recorder.config().period_time()};
  std::vector<std::string> playback_devices{Recorder::get_supporting_devices(devices, playback_config, SND_PCM_STREAM_PLAYBACK)};

  // std::cout << "----playback PCMs----------------------------------------------" << std::endl;
  // for(auto device : playback_devices) {
  //   std::cout << device << std::endl;
  // }
  // open chosen device
  device playback_device{playback_devices[0], SND_PCM_STREAM_PLAYBACK};
  if(!playback_device) return 1;

  playback_config.install(playback_device);

  // stream_buffer(std::cout, buffer, sizeof(buffer) / sizeof(*buffer));

  uint8_t* buffer = recorder.buffer(); 
  for(uint8_t* start = &buffer[0]; start < &buffer[recorder.buffer_bytes()]; start += playback_config.period_bytes()) {
    int err = snd_pcm_writei(playback_device, start, playback_config.period_frames());
    if(err == -EPIPE) {
      snd_pcm_prepare(playback_device);
    }
    else if(err != int(playback_config.period_frames())) {
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