#ifndef CONFIG_HPP
#define CONFIG_HPP

#include </usr/include/alsa/asoundlib.h>
#include <utility>

class Config {
 public:
  Config(unsigned chan = 2, unsigned frames = 44100, unsigned period_time = 23219);

  Config();
  Config(Config&& c);
  Config(Config const& c);

  ~Config();

  Config& operator=(Config c);

  void install(snd_pcm_t* pcm_handle);

  snd_pcm_hw_params_t* params();

  bool is_supported(snd_pcm_t* pcm_handle) const;

  void set_period_time(unsigned time);
  
  // returns the min and max time of a period in us
  std::pair<unsigned, unsigned> period_time_extremes() const;

  unsigned channels() const;
  
  // return size of buffer required to record one period
  unsigned period_bytes() const;

  // return time a period takes in us
  unsigned period_time() const;

  // return number of samples during one period
  unsigned period_frames() const;

  // returns size of buffer required to record time of useconds
  unsigned long buffer_bytes(unsigned long useconds) const;

  friend void swap(Config& a, Config& b);

 private:
  bool configure(snd_pcm_t* pcm_handle);
  
  unsigned channels_;
  unsigned framerate_;
  unsigned period_time_;
  snd_pcm_format_t format_;
  snd_pcm_access_t access_;
  snd_pcm_hw_params_t* params_;
};

#endif //CONFIG_HPP 
