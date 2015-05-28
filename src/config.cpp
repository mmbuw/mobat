#include <iostream>
#include "config.hpp"

Config::Config(unsigned chan, unsigned frames) :
  channels_{chan},
  framerate_{frames},
  access_{SND_PCM_ACCESS_RW_INTERLEAVED},
  format_{SND_PCM_FORMAT_S32_LE}
{
  int err = snd_pcm_hw_params_malloc(&params_);
  if(err < 0) {
    std::cerr << "cannot allocate hardware parameter structure - " << snd_strerror(err) << std::endl;
  }
}

Config::~Config() {
  snd_pcm_hw_params_free(params_);
}

void Config::install(snd_pcm_t* pcm_handle) {
  if(configure(pcm_handle)) {
    // install configuration, calls "snd_pcm_prepare" on handle automatically
    int err = snd_pcm_hw_params(pcm_handle, params_);
    if(err < 0) {
      std::cerr << "cannot set parameters - " << snd_strerror(err) << std::endl;
    }
  }
}

snd_pcm_hw_params_t* Config::params() {
  return params_;
}

bool Config::configure(snd_pcm_t* pcm_handle) {
  if(!pcm_handle) {
    std::cerr << "Config::is_supported - device not initialized" << std::endl;
    return false;
  }
       
  int err = snd_pcm_hw_params_any(pcm_handle, params_);
  if(err < 0) {
    std::cerr << "cannot initialize hardware parameter structure - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_set_access(pcm_handle, params_, access_);
  if(err < 0) {
    std::cerr << "cannot set access type - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_set_format(pcm_handle, params_, format_);
  if(err < 0) {
    std::cerr << "cannot set sample format - " << snd_strerror(err) << std::endl;
    return false;
  }
  unsigned new_framerate = framerate_;
  err = snd_pcm_hw_params_set_rate_near(pcm_handle, params_, &new_framerate, 0);
  if(err < 0) {
    std::cerr << "cannot set sample rate - " << snd_strerror(err) << std::endl;
    return false;
  }
  else if(new_framerate != framerate_) {
    std::cerr << "Adjusted sample rate from " << framerate_ << " to " << new_framerate << std::endl;
  }
  // else {
  //   std::cout << "Rate set to " << new_framerate << std::endl;
  // }

  err = snd_pcm_hw_params_set_channels(pcm_handle, params_, channels_);
  if(err < 0) {
    std::cerr << "cannot set channel count - " << snd_strerror(err) << std::endl;
    return false;
  }
  return true;
}

bool Config::is_supported(snd_pcm_t* pcm_handle) const{
  if(!pcm_handle) {
    std::cerr << "Config::is_supported - device not initialized" << std::endl;
    return false;
  }
       
  int err = snd_pcm_hw_params_any(pcm_handle, params_);
  if(err < 0) {
    std::cerr << "cannot initialize hardware parameter structure - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_access(pcm_handle, params_, access_);
  if(err < 0) {
    std::cerr << "cannot set access type - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_format(pcm_handle, params_, format_);
  if(err < 0) {
    std::cerr << "cannot set sample format - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_rate(pcm_handle, params_, framerate_, 0);
  if(err < 0) {
    std::cerr << "cannot set sample rate - " << snd_strerror(err) << std::endl;
    return false;
  }
  // else {
  //   std::cout << "Rate set to " << new_framerate << std::endl;
  // }

  err = snd_pcm_hw_params_test_channels(pcm_handle, params_, channels_);
  if(err < 0) {
    std::cerr << "cannot set channel count - " << snd_strerror(err) << std::endl;
    return false;
  }
  return true;
}

unsigned Config::channels() const {
  return channels_;
}

unsigned Config::period_bytes() const {
  // frames per period * channels * samplesize(depending on format)
  int sample_bits = snd_pcm_hw_params_get_sbits(params_);
  if(sample_bits < 0) {
    std::cerr << "no format specified, cant computer period size - " << snd_strerror(sample_bits) << std::endl;
  }
  return period_frames() * channels_ * sample_bits;
}

unsigned Config::period_time() const {
  // how long a period takes in us
  unsigned period_time;
  snd_pcm_hw_params_get_period_time(params_, &period_time, 0);
  return period_time;
}

unsigned Config::period_frames() const {
  snd_pcm_uframes_t num_frames;
  // number of frames per period
  snd_pcm_hw_params_get_period_size(params_, &num_frames, 0);
  return num_frames;
}

unsigned long Config::buffer_bytes(unsigned long useconds) const {
  // extra brackets necessary, otherwise result is totally wrong
  return period_bytes() * (useconds / period_time());
}