#include <iostream>
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

int main (int argc, char *argv[])
{
  int i;
  int err;
  short buf[128];
  snd_pcm_t *playback_handle;
  snd_pcm_hw_params_t *hw_params;

  if ((err = snd_pcm_open (&playback_handle, argv[1], SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
       argv[1],
       snd_strerror (err));
    exit (1);
  }
  std::cout << "Hello" << std::endl;
  return 0;
}