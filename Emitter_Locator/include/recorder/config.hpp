#ifndef CONFIG_HPP
#define CONFIG_HPP

#include </usr/include/alsa/asoundlib.h>
#include <utility>

class Config {
 public:
  Config(unsigned chan = 2, std::size_t frames = 44100, std::size_t period_time = 23219);
  Config();
  Config(Config&& c);
  Config(Config const& c);

  ~Config();

  Config& operator=(Config c);

  // calls configure(), if successfull, installs this config and changes internal params to fit device
  void install(snd_pcm_t* pcm_handle);

  // check if device supports this configuration
  bool isSupported(snd_pcm_t* pcm_handle) const;
  // set period time, for configuration after tavaiible times are known
  void setPeriodTime(std::size_t time);
  
  // returns the min and max time of a period in us
  std::pair<std::size_t, std::size_t> periodTimeExtremes() const;
  
  // returns undelying hardware params
  snd_pcm_hw_params_t const* params() const;
  // returns number of channels
  unsigned channels() const;
  
  // return size of buffer required to record one period
  std::size_t periodBytes() const;

  // return time a period takes in us
  std::size_t periodTime() const;

  // return number of samples during one period
  std::size_t periodFrames() const;

  // returns size of buffer required to record time of useconds
  std::size_t bufferBytes(std::size_t useconds) const;

  friend void swap(Config& a, Config& b);

 private:
  // sets configurations for device, may change internal params  
  bool configure(snd_pcm_t* pcm_handle);
  
  unsigned channels_;
  std::size_t framerate_;
  std::size_t period_time_;
  snd_pcm_format_t format_;
  snd_pcm_access_t access_;
  snd_pcm_hw_params_t* params_;
};

#endif //CONFIG_HPP 
