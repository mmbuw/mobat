#include <iostream>
#include <vector>
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

int main(int argc, char *argv[])
{
  int err;

  std::cout << "----Cards --------------------------------------------------" << std::endl;
  int card_index = -1;
  char* card_name;

  err = snd_card_next(&card_index);
  // iterate over cards until method fails
  while(card_index > -1) {
    if(err != 0) {
      std::cerr << "cannot get next audio card " << snd_strerror(err) << std::endl;
    }

    err = snd_card_get_longname(card_index, &card_name);
    if(err != 0) {
      std::cerr << "cannot get audio card name " << snd_strerror(err) << std::endl;
      return 1;
    }
    std::cout << card_name << std::endl;
    // create id string for card
    char card_id[32];
    sprintf(card_id, "hw:%d", card_index);
    // control handle
    snd_ctl_t *card_handle;
    err = snd_ctl_open(&card_handle, card_id, 0);
    if (err < 0) {
      std::cerr << "cannot open control (" << card_index << ") : " << snd_strerror(err) << std::endl;
      return 1;
    }

    //get sound card info 
    snd_ctl_card_info_t* card_info;
    snd_ctl_card_info_alloca(&card_info);
    err = snd_ctl_card_info(card_handle, card_info);
    if(err < 0) {
      std::cerr << "cannot open control hardware info (" << card_index << ") :" << snd_strerror(err) << std::endl;
      snd_ctl_close(card_handle);
      return 1;
    }

    int device_index = -1;
    err = snd_ctl_pcm_next_device(card_handle, &device_index);
    while(device_index > -1) {
      if(err != 0) {
        std::cerr << "cannot get next pcm device " << snd_strerror(err) << std::endl;
      }
      // configure info struct
      snd_pcm_info_t* pcm_info;
      snd_pcm_info_alloca(&pcm_info);

      snd_pcm_info_set_device(pcm_info, device_index);
      snd_pcm_info_set_subdevice(pcm_info, 0);
      snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_CAPTURE);
      
      err = snd_ctl_pcm_info(card_handle, pcm_info);
      if(err < 0) {
        std::cerr << "cannot get control digital audio info (" << card_index << "," << device_index << "):" << snd_strerror(err) << std::endl;
        return 1;
      }

      printf("card %i: %s [%s], device %i: %s [%s]\n",
                        card_index,
                        snd_ctl_card_info_get_id(card_info),
                        snd_ctl_card_info_get_name(card_info),
                        device_index,
                        snd_pcm_info_get_id(pcm_info),
                        snd_pcm_info_get_name(pcm_info));

      err = snd_ctl_pcm_next_device(card_handle, &device_index);
    } ;
    


    snd_ctl_close(card_handle);
    free(card_name);
    
    err = snd_card_next(&card_index);
  }

  std::cout << "----Devices --------------------------------------------------" << std::endl;
  char** hints;

  err = snd_device_name_hint(-1, "pcm",(void***)&hints);
  if(err != 0) {
    std::cerr << "cannot open audio device list " << snd_strerror(err) << std::endl;
    return 1;
  }

  std::vector<char*> devices{};
  for(char** n = hints; *n != NULL; ++n) {
    char* name = snd_device_name_get_hint(*n, "NAME");
    char* io = snd_device_name_get_hint(*n, "IOID");

    if(name != NULL && strcmp("null", name) != 0) {
      if(io == NULL || strcmp("Output", io) != 0) {
        devices.push_back(name);
      }
      free(name);
      free(io);
    }
  }
  snd_device_name_free_hint((void**)hints);

  for(auto device : devices) {
    std::cout << device << std::endl;
  }

  snd_pcm_t *capture_handle;
  err = snd_pcm_open(&capture_handle, devices[0], SND_PCM_STREAM_CAPTURE, 0);
  if(err < 0) {
    std::cerr << "cannot open audio device " << argv[1] << " " << snd_strerror(err) << std::endl;
    return 1;
  }
     
  snd_pcm_hw_params_t *hw_params;
  err = snd_pcm_hw_params_malloc(&hw_params);
  if(err < 0) {
    std::cerr << "cannot allocate hardware parameter structure " << snd_strerror(err) << std::endl;
    return 1;
  }
       
  err = snd_pcm_hw_params_any(capture_handle, hw_params);
  if(err < 0) {
    std::cerr << "cannot initialize hardware parameter structure " << snd_strerror(err) << std::endl;
    return 1;
  }

  err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
  if(err < 0) {
    std::cerr << "cannot set access type " << snd_strerror(err) << std::endl;
    return 1;
  }

  err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S32_LE);
  if(err < 0) {
    std::cerr << "cannot set sample format " << snd_strerror(err) << std::endl;
    return 1;
  }

  unsigned rate = 48000;
  err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
  if(err < 0) {
    std::cerr << "cannot set sample rate " << snd_strerror(err) << std::endl;
    return 1;
  }

  err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 4);
  if(err < 0) {
    std::cerr << "cannot set channel count " << snd_strerror(err) << std::endl;
    return 1;
  }

  err = snd_pcm_hw_params(capture_handle, hw_params);
  if(err < 0) {
    std::cerr << "cannot set parameters " << snd_strerror(err) << std::endl;
    return 1;
  }

  snd_pcm_hw_params_free(hw_params);

  err = snd_pcm_prepare(capture_handle);
  if(err < 0) {
    std::cerr << "cannot prepare audio interface for use " << snd_strerror(err) << std::endl;
    return 1;
  }

  short buf[128];
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