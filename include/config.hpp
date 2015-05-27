// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

class Config {
 public:
  Config(unsigned chan = 2, unsigned frames = 44000);

  ~Config();

  void install(snd_pcm_t* pcm_handle);

  snd_pcm_hw_params_t* params();

  bool test(snd_pcm_t* pcm_handle) const;

  unsigned channels() const;

  unsigned period_bytes() const;

  unsigned period_time() const;

  unsigned period_frames() const;

 private:
  bool configure(snd_pcm_t* pcm_handle);
  
  unsigned channels_;
  unsigned framerate_;
  snd_pcm_format_t format_;
  snd_pcm_access_t access_;
  snd_pcm_hw_params_t* params_;
};
