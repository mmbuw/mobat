#include "config.hpp"
#include <iostream>

Config::Config() :
  channels_{0},
  framerate_{0},
  period_time_{0},
  format_{SND_PCM_FORMAT_UNKNOWN},
  access_{SND_PCM_ACCESS_RW_INTERLEAVED},
  params_{nullptr}
{}

Config::Config(unsigned chan, std::size_t frames, std::size_t period_time) :
  channels_{chan},
  framerate_{frames},
  period_time_{period_time},
  format_{SND_PCM_FORMAT_S32_LE},
  access_{SND_PCM_ACCESS_RW_INTERLEAVED},
  params_{nullptr}
{
  int err = snd_pcm_hw_params_malloc(&params_);
  if(err < 0) {
    std::cerr << "cannot allocate hardware parameter structure - " << snd_strerror(err) << std::endl;
  }
}

Config::Config(Config&& c) {
  swap(*this, c);
}

Config::Config(Config const& c) :
  channels_{c.channels_},
  framerate_{c.framerate_},
  period_time_{c.period_time_},
  format_{c.format_},
  access_{c.access_},
  params_{nullptr}
{
   snd_pcm_hw_params_copy(params_, c.params_);
}

Config::~Config() {
  snd_pcm_hw_params_free(params_);
}

Config& Config::operator=(Config c) {
  swap(*this, c);
  return *this;
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
    std::cerr << "Config::configure - device not initialized" << std::endl;
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

  err = snd_pcm_hw_params_set_channels(pcm_handle, params_, channels_);
  if(err < 0) {
    std::cerr << "cannot set channel count - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_set_period_time(pcm_handle, params_, period_time_, 0);
  if(err < 0) {
    std::cerr << "cannot set period time - " << snd_strerror(err) << std::endl;
    return false;
  }

  return true;
}

bool Config::is_supported(snd_pcm_t* pcm_handle) const{
  if(!pcm_handle) {
    // std::cerr << "Config::is_supported - device not initialized" << std::endl;
    return false;
  }

  int err = snd_pcm_hw_params_any(pcm_handle, params_);
  if(err < 0) {
    // std::cerr << "cannot initialize hardware parameter structure - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_access(pcm_handle, params_, access_);
  if(err < 0) {
    // std::cerr << "cannot set access type - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_format(pcm_handle, params_, format_);
  if(err < 0) {
    // std::cerr << "cannot set sample format - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_rate(pcm_handle, params_, framerate_, 0);
  if(err < 0) {
    // std::cerr << "cannot set sample rate - " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_test_channels(pcm_handle, params_, channels_);
  if(err < 0) {
    // std::cerr << "cannot set channel count - " << snd_strerror(err) << std::endl;
    return false;
  }

  return true;
}

void Config::set_period_time(std::size_t time) {
  period_time_ = time;
}

std::pair<std::size_t, std::size_t> Config::period_time_extremes() const {
  unsigned int min = 0;
  int err = snd_pcm_hw_params_get_period_time_min(params_, &min, 0);
  if(err < 0) {
    std::cerr << "cannot get min period time - " << snd_strerror(err) << std::endl;
    return std::make_pair(0, 0);
  }

  unsigned int max = 0;
  err = snd_pcm_hw_params_get_period_time_max(params_, &max, 0);
  if(err < 0) {
    std::cerr << "cannot get max period time - " << snd_strerror(err) << std::endl;
    return std::make_pair(0, 0);
  }

  return std::make_pair(min, max);
}

unsigned Config::channels() const {
  return channels_;
}

std::size_t Config::period_bytes() const {
  // frames per period * channels * samplesize(depending on format)
  int sample_bytes = snd_pcm_hw_params_get_sbits(params_) / 8;
  if(sample_bytes < 0) {
    std::cerr << "no format specified, cant compute period size - " << snd_strerror(sample_bytes) << std::endl;
  }
  return period_frames() * channels_ * sample_bytes;
}

std::size_t Config::period_time() const {
  // how long a period takes in us
  unsigned period_time;
  snd_pcm_hw_params_get_period_time(params_, &period_time, 0);
  return period_time;
}

std::size_t Config::period_frames() const {
  snd_pcm_uframes_t num_frames;
  // number of frames per period
  snd_pcm_hw_params_get_period_size(params_, &num_frames, 0);
  return num_frames;
}

std::size_t Config::buffer_bytes(std::size_t useconds) const {
  // extra brackets necessary, otherwise result is totally wrong
  return period_bytes() * (useconds / period_time());
}

 void swap(Config& a, Config& b) {
  std::swap(a.channels_, b.channels_);
  std::swap(a.framerate_, b.framerate_);
  std::swap(a.period_time_, b.period_time_);
  std::swap(a.format_, b.format_);
  std::swap(a.access_, b.access_);
  std::swap(a.params_, b.params_);
  }