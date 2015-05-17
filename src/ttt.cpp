#include <iostream>
#include <vector>
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

int main(int argc, char *argv[])
{
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
      return 1;
    }

    //get sound card info 
    err = snd_ctl_card_info(card_handle, card_info);
    if(err < 0) {
      std::cerr << "cannot open control hardware info (" << card_index << ") :" << snd_strerror(err) << std::endl;
      snd_ctl_close(card_handle);
      return 1;
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
      snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_CAPTURE);
      
      err = snd_ctl_pcm_info(card_handle, pcm_info);
      if(err < 0) {
        std::cerr << "cannot get control digital audio info (" << card_index << "," << device_index << "):" << snd_strerror(err) << std::endl;
        return 1;
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

  std::cout << "----PCMs --------------------------------------------------" << std::endl;
  char** hints;

  err = snd_device_name_hint(-1, "pcm",(void***)&hints);
  if(err != 0) {
    std::cerr << "cannot open audio device list " << snd_strerror(err) << std::endl;
    return 1;
  }

  std::vector<std::string> devices{};
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

  for(auto device : devices) {
    std::cout << std::string{device} << std::endl;
  }

  std::cout << "----testing PCMs----------------------------------------------" << std::endl;
  unsigned num_channels = 4;
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
  err = snd_pcm_hw_params_malloc(&hw_params);
  if(err < 0) {
    std::cerr << "cannot allocate hardware parameter structure " << snd_strerror(err) << std::endl;
    return 1;
  }

  std::vector<std::string> correct_devices{};
  for(auto const& device : devices) {
    err = snd_pcm_open(&capture_handle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if(err < 0) {
      std::cerr << "cannot open audio device " << device<< " " << snd_strerror(err) << std::endl;
      continue;
    }
    else {
      std::cout << "Starting configuration on " << device << std::endl;
    }
         
    err = snd_pcm_hw_params_any(capture_handle, hw_params);
    if(err < 0) {
      std::cerr << "cannot initialize hardware parameter structure " << snd_strerror(err) << std::endl;
      continue;
    }

    err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(err < 0) {
      std::cerr << "cannot set access type " << snd_strerror(err) << std::endl;
      continue;
    }

    err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S32_LE);
    if(err < 0) {
      std::cerr << "cannot set sample format " << snd_strerror(err) << std::endl;
      continue;
    }

    unsigned rate = 48000;
    err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
    if(err < 0) {
      std::cerr << "cannot set sample rate " << snd_strerror(err) << std::endl;
      continue;
    }

    err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, num_channels);
    if(err < 0) {
      std::cerr << "cannot set channel count " << snd_strerror(err) << std::endl;
      continue;
    }

    correct_devices.push_back(device);
  }

  std::cout << "----suitable PCMs----------------------------------------------" << std::endl;
  for(auto device : correct_devices) {
    std::cout << device << std::endl;
  }
  // install configuration, calls "snd_pcm_prepare" on handle automatically
  err = snd_pcm_hw_params(capture_handle, hw_params);
  if(err < 0) {
    std::cerr << "cannot set parameters " << snd_strerror(err) << std::endl;
    return 1;
  }

  snd_pcm_hw_params_free(hw_params);

  // 8 * sample_format_bits * num_channels
  unsigned buffer_size = 8 * 32 * num_channels;
  short buf[buffer_size];
  for(unsigned i = 0; i < 10; ++i) {
    err = snd_pcm_readi(capture_handle, buf, 128);
    if(err != 128) {
      std::cerr << "read from audio interface failed " << snd_strerror(err) << std::endl;
      return 1;
    }
  }

  snd_pcm_close(capture_handle);
  std::cout << "success" << std::endl;


  return 0;
}