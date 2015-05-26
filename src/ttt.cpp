#include <iostream>
#include <vector>
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

void output_cards();
std::vector<std::string> get_pcms();

class config {
 public:

  config(unsigned chan = 2, unsigned frames = 44000, snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK) :
    channels{chan},
    framerate{frames},
    access{SND_PCM_ACCESS_RW_INTERLEAVED},
    format{SND_PCM_FORMAT_S16_LE}
  {
    int err = snd_pcm_hw_params_malloc(&hw_params);
    if(err < 0) {
      std::cerr << "cannot allocate hardware parameter structure " << snd_strerror(err) << std::endl;
    }
  }

  ~config() {
    snd_pcm_hw_params_free(hw_params);
  }

  void install(snd_pcm_t* pcm_handle) {
    // install configuration, calls "snd_pcm_prepare" on handle automatically
    int err = snd_pcm_hw_params(pcm_handle, hw_params);
    if(err < 0) {
      std::cerr << "cannot set parameters " << snd_strerror(err) << std::endl;
    }
  }

  snd_pcm_hw_params_t* get_params() {
    return hw_params;
  }

  bool supports_params(snd_pcm_t* pcm_handle) {

    // err = snd_pcm_open(&capture_handle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    // if(err < 0) {
    //   std::cerr << "cannot open audio device " << device<< " " << snd_strerror(err) << std::endl;
    //   return false;
    // }
    // else {
    //   std::cout << "Starting configuration on " << device << std::endl;
    // }
         
    int err = snd_pcm_hw_params_any(pcm_handle, hw_params);
    if(err < 0) {
      std::cerr << "cannot initialize hardware parameter structure " << snd_strerror(err) << std::endl;
      return false;
    }

    err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, access);
    if(err < 0) {
      std::cerr << "cannot set access type " << snd_strerror(err) << std::endl;
      return false;
    }

    err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, format);
    if(err < 0) {
      std::cerr << "cannot set sample format " << snd_strerror(err) << std::endl;
      return false;
    }
    unsigned new_framerate = framerate;
    err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &new_framerate, 0);
    if(err < 0) {
      std::cerr << "cannot set sample rate " << snd_strerror(err) << std::endl;
      return false;
    }
    // else {
    //   std::cout << "Rate set to " << new_framerate << std::endl;
    // }

    err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, channels);
    if(err < 0) {
      std::cerr << "cannot set channel count " << snd_strerror(err) << std::endl;
      return false;
    }

    return true;
  }

  unsigned get_channels() {
    return channels;
  }

  unsigned get_period_bytes() {
    snd_pcm_uframes_t num_frames;
    // number of frames per period
    snd_pcm_hw_params_get_period_size(hw_params, &num_frames, 0);
    // frames per period * channels * samplesize(2 Byte)
    return num_frames * channels * 2;
  }

  unsigned get_period_time() {
    // how long a period takes in us
    unsigned period_time;
    snd_pcm_hw_params_get_period_time(hw_params, &period_time, 0);
    return period_time;
  }

  unsigned get_period_frames() {
    snd_pcm_uframes_t num_frames;
    // number of frames per period
    snd_pcm_hw_params_get_period_size(hw_params, &num_frames, 0);
    return num_frames;
  }

 private:
  unsigned channels;
  unsigned framerate;
  snd_pcm_format_t format;
  snd_pcm_access_t access;
  snd_pcm_stream_t stream;
  snd_pcm_hw_params_t* hw_params;
};

int main(int argc, char *argv[])
{
  output_cards();

  int err;

  std::vector<std::string> devices{get_pcms()};

  for(auto device : devices) {
    std::cout << std::string{device} << std::endl;
  }

  std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  snd_pcm_t *capture_handle;
  config capture_config{2, 48000};

  std::vector<std::string> capture_devices{};
  for(auto const& device : devices) {
    err = snd_pcm_open(&capture_handle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if(err < 0) {
      std::cerr << "cannot open audio device " << device<< " " << snd_strerror(err) << std::endl;
      continue;
    }
    else {
      std::cout << "Starting configuration on " << device << std::endl;
    }
         
    if(capture_config.supports_params(capture_handle)) {
     capture_devices.push_back(device);  
    }
  }

  std::cout << "----recording PCMs----------------------------------------------" << std::endl;
  for(auto device : capture_devices) {
    std::cout << device << std::endl;
  }
  // open chosen device
  std::string capture_device{capture_devices[0]};
  err = snd_pcm_open(&capture_handle, capture_device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
  if(err < 0) {
    std::cerr << "cannot open audio device " << capture_device<< " " << snd_strerror(err) << std::endl;
    return 1;
  }

  capture_config.install(capture_handle);


  std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  config playback_config{2, 48000};
  snd_pcm_t *playback_handle;
  std::vector<std::string> playback_devices{};
  for(auto const& device : devices) {
    err = snd_pcm_open(&playback_handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if(err < 0) {
      std::cerr << "cannot open audio device " << device<< " " << snd_strerror(err) << std::endl;
      continue;
    }
    else {
      std::cout << "Starting configuration on " << device << std::endl;
    }
         
    if(playback_config.supports_params(playback_handle)) {
      playback_devices.push_back(device);
    }
  }

  std::cout << "----playback PCMs----------------------------------------------" << std::endl;
  for(auto device : playback_devices) {
    std::cout << device << std::endl;
  }
  // open chosen device
  std::string playback_device{devices[0]};
  err = snd_pcm_open(&playback_handle, playback_device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
  if(err < 0) {
    std::cerr << "cannot open audio device " << playback_device<< " " << snd_strerror(err) << std::endl;
    return 1;
  }

  playback_config.install(playback_handle);
  snd_pcm_start(playback_handle);

  char* buf;
  buf = (char*)malloc(capture_config.get_period_bytes());
  unsigned seconds = 2;
  for(int loop = seconds * 1000000 / capture_config.get_period_time(); loop > 0; --loop) {
  std::cout << loop << std::endl;
  //   err = snd_pcm_readi(capture_handle, buf, capture_config.get_period_bytes());
  //   if(err != capture_config.get_period_bytes()) {
  //     std::cerr << "read from audio interface failed " << snd_strerror(err) << std::endl;
  //     return 1;
  //   }
  // std::cout << "finish record" << std::endl;
    if (err = read(0, buf, capture_config.get_period_bytes()) == 0) {
      printf("Early end of file.\n");
      return 0;
    }
    err = snd_pcm_writei(playback_handle, buf, playback_config.get_period_frames());
    if(err == -EPIPE) {
      snd_pcm_prepare(playback_handle);
    }
    else if (err != capture_config.get_period_frames()) {
      std::cerr << "write to audio interface failed " << snd_strerror(err) << std::endl;
      return 1;
    }
  }

  snd_pcm_close(capture_handle);
  snd_pcm_drain(playback_handle);
  snd_pcm_close(playback_handle);

  free(buf);
  return 0;
}

std::vector<std::string> get_pcms() {
  int err;
  std::vector<std::string> devices{};

  std::cout << "----PCMs --------------------------------------------------" << std::endl;
  char** hints;

  err = snd_device_name_hint(-1, "pcm",(void***)&hints);
  if(err != 0) {
    std::cerr << "cannot open audio device list " << snd_strerror(err) << std::endl;
    return devices;
  }

  for(char** n = hints; *n != NULL; ++n) {
    char* name = snd_device_name_get_hint(*n, "NAME");
    char* io = snd_device_name_get_hint(*n, "IOID");

    if(name != NULL && strcmp("null", name) != 0 && strcmp("null", name)) {
      if(io == NULL || strcmp("Output", io) != 0) {
        devices.push_back(name);
        if(io != NULL)std::cout << name << " " << std::string{io} << std::endl;
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

  std::cout << "----Cards --------------------------------------------------" << std::endl;
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
    std::cout << "Card: " << card_index
              << ", Name: \"" << card_name
              << "\", ID: " << card_id
              << std::endl;

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
      std::cout << " Device: " << device_index
          << ", Name: \"" << device_name
          << "\", ID: " << device_id
          << std::endl;

      err = snd_ctl_pcm_next_device(card_handle, &device_index);
    }

    snd_ctl_close(card_handle);
    err = snd_card_next(&card_index);
  }
  // free resources
  snd_ctl_card_info_free(card_info);
  snd_pcm_info_free(pcm_info);
}