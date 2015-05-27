// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

class Config {
 public:
  Config(unsigned chan = 2, unsigned frames = 44000);

  ~Config();

  void install(snd_pcm_t* pcm_handle) const;

  snd_pcm_hw_params_t* get_params();

  bool configure(snd_pcm_t* pcm_handle);

  unsigned get_channels() const;

  unsigned get_period_bytes() const;

  unsigned get_period_time() const;

  unsigned get_period_frames() const;

 private:
  unsigned channels;
  unsigned framerate;
  snd_pcm_format_t format;
  snd_pcm_access_t access;
  snd_pcm_stream_t stream;
  snd_pcm_hw_params_t* params;
};
