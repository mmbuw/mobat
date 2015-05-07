#include <iostream>
#include <vector>
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

int main (int argc, char *argv[])
{
  int i;
  int err;

  std::cout << "----Cards --------------------------------------------------" << std::endl;
  int card = -1;
  char* name;

  err = snd_card_next(&card);
  // iterate over cards until method fails
  while(card > -1) {
    if(err != 0) {
      std::cerr << "cannot get next audio card " << snd_strerror(err) << std::endl;
    }

    err = snd_card_get_longname(card, &name);
    if(err != 0) {
      std::cerr << "cannot get audio card name " << snd_strerror(err) << std::endl;
      return 1;
    }
    std::cout << name << std::endl;
    free(name);
    
    err = snd_card_next(&card);
  }

  std::cout << "----Devices --------------------------------------------------" << std::endl;
  char** hints;

  err = snd_device_name_hint(-1, "pcm", (void***)&hints);
  if(err != 0) {
    std::cerr << "cannot open audio device list " << snd_strerror(err) << std::endl;
    return 1;
  }

  char** n = hints;
  while(*n != NULL) {
    char* name = snd_device_name_get_hint(*n, "NAME");
    char* desc = snd_device_name_get_hint(*n, "DESC");
    char* io = snd_device_name_get_hint(*n, "IOID");

    if(name != NULL && strcmp("null", name) != 0) {
      std::cout << name << std::endl;
      // std::cout << " " << desc << std::endl;
      // std::cout << " " << io << std::endl;
      free(name);
      free(desc);
      free(io);
    }
    ++n; 
  }
  snd_device_name_free_hint((void**)hints);
  
  // snd_pcm_t *capture_handle;
  // if ((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
  //   std::cerr << "cannot open audio device " << argv[1] << " " << snd_strerror(err) << std::endl;
  //   return 1;
  // }
     
  // snd_pcm_hw_params_t *hw_params;
  // if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
  //   std::cerr << "cannot allocate hardware parameter structure " << snd_strerror(err) << std::endl;
  //   return 1;
  // }
       
  // if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
  //   std::cerr << "cannot initialize hardware parameter structure " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
  //   std::cerr << "cannot set access type " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S32_LE)) < 0) {
  //   std::cerr << "cannot set sample format " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // unsigned rate = 48000;
  // err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0);
  // if (err < 0) {
  //   std::cerr << "cannot set sample rate " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
  //   std::cerr << "cannot set channel count " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
  //   std::cerr << "cannot set parameters " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // snd_pcm_hw_params_free (hw_params);

  // if ((err = snd_pcm_prepare (capture_handle)) < 0) {
  //   std::cerr << "cannot prepare audio interface for use " << snd_strerror(err) << std::endl;
  //   return 1;
  // }

  // short buf[128];
  // for (i = 0; i < 10; ++i) {
  //   if ((err = snd_pcm_readi (capture_handle, buf, 128)) != 128) {
  //     std::cerr << "read from audio interface failed " << snd_strerror(err) << std::endl;
  //     return 1;
  //   }
  // }

  // snd_pcm_close (capture_handle);
  return 0;
}