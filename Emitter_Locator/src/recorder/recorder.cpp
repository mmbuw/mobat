#include "recorder.hpp"

Recorder::Recorder(unsigned chan, unsigned frames, unsigned recording_time) :
  config_{chan, frames, 0},
  device_{},
  recording_time_{recording_time},
  buffer_length_{0}
{
  std::vector<std::string> devices{get_pcms()};

  std::vector<std::string> capture_devices{get_supporting_devices(devices, config_, SND_PCM_STREAM_CAPTURE)};

  std::string capture_name{capture_devices[1]};
  for(auto device : capture_devices) {

    //check for strings that contain the behringer id string
    //other device
    std::string::size_type behringer_substring_pos 
      = device.find("hw:CARD=UMC404,DEV=0");
    if( std::string::npos != behringer_substring_pos ) {
      capture_name = device;
      break;
    }
  }
  if(capture_name == capture_devices[0]) {
    std::cerr << "Behringer UMC not found, using default recording device" << std::endl;
  }
  // open chosen device
  device_ =  device{"default", SND_PCM_STREAM_CAPTURE};
  if(!device_) {
    std::cerr << "no usable device found" << std::endl;
    return;
  }

  auto extremes = config_.period_time_extremes();
  unsigned period_time = extremes.second;
  // find largest period size fitting in recording buffer
  if(recording_time_ < period_time) {
    period_time = recording_time_;
  }
  else {
    while(recording_time_ % period_time != 0) {
      --period_time; 
    }
  }
  // is no usable time was found recoding is impossible
  if(period_time < extremes.first) {
    std::cerr << "could not find period matching recording time" << std::endl;
    return;
  }
  config_.set_period_time(period_time);
  // install configuration and allocate buffer
  config_.install(device_);
  buffer_length_ = config_.buffer_bytes(recording_time_);
  buffer_ = new unsigned char[buffer_length_];
}

Recorder::~Recorder() {
  delete [] buffer_;
}

std::string const& Recorder::device_name() const {
  return device_.name;
}
Config& Recorder::config() {
  return config_;
}

void Recorder::record() {

  // prevent under- or overruns
  unsigned loops = recording_time_ / config_.period_time();
  if(loops * config_.period_bytes() > buffer_length_) {
    std::cerr << "Recorder::record - buffer size to small" << std::endl;
    return;
  }
  else if(loops * config_.period_bytes() < buffer_length_) {
    std::cerr << "Recorder::record - buffer size too large" << std::endl;
  }

  for(unsigned char* start = &buffer_[0]; start < &buffer_[buffer_length_ / sizeof(*buffer_)]; start += config_.period_bytes()) {
    int err = snd_pcm_readi(device_, start, config_.period_frames());
    if(err != int(config_.period_frames())) {
      std::cerr << "read from audio interface failed " << snd_strerror(err) << std::endl;
      return;
    }
  }  
}

unsigned long Recorder::buffer_bytes() const {
  return buffer_length_;
}

unsigned char* Recorder::buffer() {
  return buffer_;
}

std::vector<std::string> Recorder::get_supporting_devices(std::vector<std::string> const& devices, Config const& config, snd_pcm_stream_t type) {
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

std::vector<std::string> Recorder::get_pcms() {
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
        // if(io != nullptr)std::cout << name << " " << std::string{io} << std::endl;
      }
      free(name);
      free(io);
    }
  }

  snd_device_name_free_hint((void**)hints);
  return devices;
}

void Recorder::output_cards() {
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

    // const char* card_name = snd_ctl_card_info_get_name(card_info);
    // const char* card_id = snd_ctl_card_info_get_id(card_info);
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

      // const char* device_name = snd_pcm_info_get_name(pcm_info);
      // const char* device_id = snd_pcm_info_get_id(pcm_info);
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