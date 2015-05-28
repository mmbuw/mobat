#include <iostream>
#include <vector>
#include "config.hpp"
#include "device.hpp"

void output_cards();
std::vector<std::string> get_pcms();
snd_pcm_t* open_device(std::string const&, snd_pcm_stream_t);
std::vector<std::string> get_supporting_devices(std::vector<std::string> const&, Config const&, snd_pcm_stream_t);
void stream_buffer(std::ostream&, unsigned char[], unsigned size);

int main(int argc, char *argv[])
{
  // output_cards();

  int err;

  std::vector<std::string> devices{get_pcms()};
  // for(auto device : devices) {
  //   std::cout << std::string{device} << std::endl;
  // }

  // std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  Config capture_config{1};
  std::vector<std::string> capture_devices{get_supporting_devices(devices, capture_config, SND_PCM_STREAM_CAPTURE)};

  // std::cout << "----recording PCMs----------------------------------------------" << std::endl;
  std::string capture_name{capture_devices[0]};
  for(auto device : capture_devices) {
    if(device == "hw:CARD=UMC404,DEV=0") {
      capture_name = device;
      break;
    }
  }
  if(capture_name == capture_devices[0]) {
    std::cerr << "Behringer UMC not found, using default recording device" << std::endl;
  }

  // open chosen device
  device capture_device{capture_name, SND_PCM_STREAM_CAPTURE};
  if(!capture_device) return 1;

  capture_config.install(capture_device);


  // std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  Config playback_config{1};
  std::vector<std::string> playback_devices{get_supporting_devices(devices, playback_config, SND_PCM_STREAM_PLAYBACK)};

  // std::cout << "----playback PCMs----------------------------------------------" << std::endl;
  // for(auto device : playback_devices) {
  //   std::cout << device << std::endl;
  // }
  // open chosen device
  device playback_device{playback_devices[0], SND_PCM_STREAM_PLAYBACK};
  if(!playback_device) return 1;

  playback_config.install(playback_device);

  unsigned seconds = 2;
  // use char array for easy iteration over data
  unsigned char buffer[capture_config.buffer_bytes(seconds * 1000000)];
  
  // prevent under- or overruns
  unsigned loops = seconds * 1000000 / capture_config.period_time();
  if(loops * capture_config.period_bytes() > sizeof(buffer)) {
    std::cerr << "buffer size to small" << std::endl;
    return 1;
  }
  else if(loops * capture_config.period_bytes() < sizeof(buffer)) {
    std::cerr << "buffer size too large" << std::endl;
  }

  for(unsigned char* start = &buffer[0]; start < &buffer[sizeof(buffer) / sizeof(*buffer)]; start += capture_config.period_bytes()) {
    err = snd_pcm_readi(capture_device, start, capture_config.period_frames());
    if(err != capture_config.period_frames()) {
      std::cerr << "read from audio interface failed " << snd_strerror(err) << std::endl;
      return 1;
    }
  }  

  stream_buffer(std::cout, buffer, sizeof(buffer) / sizeof(*buffer));

  for(unsigned char* start = &buffer[0]; start < &buffer[sizeof(buffer) / sizeof(*buffer)]; start += capture_config.period_bytes()) {
    err = snd_pcm_writei(playback_device, start, playback_config.period_frames());
    if(err == -EPIPE) {
      snd_pcm_prepare(playback_device);
    }
    else if(err != capture_config.period_frames()) {
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

std::vector<std::string> get_supporting_devices(std::vector<std::string> const& devices, Config const& config, snd_pcm_stream_t type) {
  std::vector<std::string> supporting_devices{};
  for(auto const& curr_device : devices) {
    device test_handle{curr_device, type};
    if(test_handle) {
      if(config.is_supported(test_handle)) {
       supporting_devices.push_back(curr_device);  
      }
    }
  }
  return supporting_devices;
}

std::vector<std::string> get_pcms() {
  int err;
  std::vector<std::string> devices{};

  // std::cout << "----PCMs --------------------------------------------------" << std::endl;
  char** hints;

  err = snd_device_name_hint(-1, "pcm",(void***)&hints);
  if(err != 0) {
    std::cerr << "cannot open audio device list " << snd_strerror(err) << std::endl;
    return devices;
  }

  for(char** n = hints; *n != nullptr; ++n) {
    char* name = snd_device_name_get_hint(*n, "NAME");
    char* io = snd_device_name_get_hint(*n, "IOID");

    if(name != nullptr && strcmp("null", name) != 0 && strcmp("null", name)) {
      if(io == nullptr || strcmp("Output", io) != 0) {
        devices.push_back(name);
        if(io != nullptr)std::cout << name << " " << std::string{io} << std::endl;
      }
      free(name);
      free(io);
    }
  }

  snd_device_name_free_hint((void**)hints);
  return devices;
}

void output_cards() {
  int err;

  // std::cout << "----Cards --------------------------------------------------" << std::endl;
  int card_index = -1;
  char card_ctl_id[32];

  snd_ctl_t *card_handle;
  snd_ctl_card_info_t* card_info;
  snd_ctl_card_info_malloc(&card_info);
  snd_pcm_info_t* pcm_info;
  snd_pcm_info_malloc(&pcm_info);

  err = snd_card_next(&card_index);
  // iterate over cards until method fails
  while(card_index > -1) {
    if(err != 0) {
      std::cerr << "cannot get next audio card " << snd_strerror(err) << std::endl;
    }

    // create id string for card
    sprintf(card_ctl_id, "hw:%d", card_index);
    // control handle
    err = snd_ctl_open(&card_handle, card_ctl_id, 0);
    if (err < 0) {
      std::cerr << "cannot open control (" << card_index << ") : " << snd_strerror(err) << std::endl;
      return;
    }

    //get sound card info 
    err = snd_ctl_card_info(card_handle, card_info);
    if(err < 0) {
      std::cerr << "cannot open control hardware info (" << card_index << ") :" << snd_strerror(err) << std::endl;
      snd_ctl_close(card_handle);
      return;
    }

    const char* card_name = snd_ctl_card_info_get_name(card_info);
    const char* card_id = snd_ctl_card_info_get_id(card_info);
    // std::cout << "Card: " << card_index
    //           << ", Name: \"" << card_name
    //           << "\", ID: " << card_id
    //           << std::endl;

    int device_index = -1;
    err = snd_ctl_pcm_next_device(card_handle, &device_index);
    while(device_index > -1) {
      if(err != 0) {
        std::cerr << "cannot get next pcm device " << snd_strerror(err) << std::endl;
      }

      // configure info struct
      snd_pcm_info_set_device(pcm_info, device_index);
      snd_pcm_info_set_subdevice(pcm_info, 0);
      // snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_PLAYBACK);
      
      err = snd_ctl_pcm_info(card_handle, pcm_info);
      if(err < 0) {
        if(err != -ENOENT) {
          std::cerr << "cannot get control digital audio info (" << card_index << "," << device_index << "):" << snd_strerror(err) << std::endl;
          return;
        }
        snd_ctl_close(card_handle);
        err = snd_card_next(&card_index);
        continue;
      }

      const char* device_name = snd_pcm_info_get_name(pcm_info);
      const char* device_id = snd_pcm_info_get_id(pcm_info);
      // std::cout << " Device: " << device_index
      //     << ", Name: \"" << device_name
      //     << "\", ID: " << device_id
      //     << std::endl;

      err = snd_ctl_pcm_next_device(card_handle, &device_index);
    }

    snd_ctl_close(card_handle);
    err = snd_card_next(&card_index);
  }
  // free resources
  snd_ctl_card_info_free(card_info);
  snd_pcm_info_free(pcm_info);
}