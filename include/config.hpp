#ifndef CONFIG_HPP
#define CONFIG_HPP

// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>

class Config {
 public:
  Config(unsigned chan = 2, unsigned frames = 44100);

  ~Config();

  void install(snd_pcm_t* pcm_handle);

  snd_pcm_hw_params_t* params();

  bool is_supported(snd_pcm_t* pcm_handle) const;

  unsigned channels() const;
  
  // return size of buffer required to record one period
  unsigned period_bytes() const;

  // return time a period takes in us
  unsigned period_time() const;

  // return number of samples during one period
  unsigned period_frames() const;

  // returns size of buffer required to record time of useconds
  unsigned long buffer_bytes(unsigned long useconds) const;

 private:
  bool configure(snd_pcm_t* pcm_handle);
  
  unsigned channels_;
  unsigned framerate_;
  snd_pcm_format_t format_;
  snd_pcm_access_t access_;
  snd_pcm_hw_params_t* params_;
};

#endif //CONFIG_HPP 
