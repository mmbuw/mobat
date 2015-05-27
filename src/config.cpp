#include <iostream>
#include "config.hpp"

Config::Config(unsigned chan, unsigned frames) :
  channels{chan},
  framerate{frames},
  access{SND_PCM_ACCESS_RW_INTERLEAVED},
  format{SND_PCM_FORMAT_S16_LE}
{
  int err = snd_pcm_hw_params_malloc(&params);
  if(err < 0) {
    std::cerr << "cannot allocate hardware parameter structure " << snd_strerror(err) << std::endl;
  }
}

Config::~Config() {
  snd_pcm_hw_params_free(params);
}

void Config::install(snd_pcm_t* pcm_handle) const {
  // install configuration, calls "snd_pcm_prepare" on handle automatically
  int err = snd_pcm_hw_params(pcm_handle, params);
  if(err < 0) {
    std::cerr << "cannot set parameters " << snd_strerror(err) << std::endl;
  }
}

snd_pcm_hw_params_t* Config::get_params() {
  return params;
}

bool Config::configure(snd_pcm_t* pcm_handle) {

  // err = snd_pcm_open(&capture_handle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
  // if(err < 0) {
  //   std::cerr << "cannot open audio device " << device<< " " << snd_strerror(err) << std::endl;
  //   return false;
  // }
  // else {
  //   std::cout << "Starting configuration on " << device << std::endl;
  // }
       
  int err = snd_pcm_hw_params_any(pcm_handle, params);
  if(err < 0) {
    std::cerr << "cannot initialize hardware parameter structure " << snd_strerror(err) << std::endl;
    return false;
  }
  err = snd_pcm_hw_params_set_access(pcm_handle, params, access);
  if(err < 0) {
    std::cerr << "cannot set access type " << snd_strerror(err) << std::endl;
    return false;
  }

  err = snd_pcm_hw_params_set_format(pcm_handle, params, format);
  if(err < 0) {
    std::cerr << "cannot set sample format " << snd_strerror(err) << std::endl;
    return false;
  }
  unsigned new_framerate = framerate;
  err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &new_framerate, 0);
  if(err < 0) {
    std::cerr << "cannot set sample rate " << snd_strerror(err) << std::endl;
    return false;
  }
  // else {
  //   std::cout << "Rate set to " << new_framerate << std::endl;
  // }

  err = snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
  if(err < 0) {
    std::cerr << "cannot set channel count " << snd_strerror(err) << std::endl;
    return false;
  }

  return true;
}

unsigned Config::get_channels() const {
  return channels;
}

unsigned Config::get_period_bytes() const {
  snd_pcm_uframes_t num_frames;
  // number of frames per period
  snd_pcm_hw_params_get_period_size(params, &num_frames, 0);
  // frames per period * channels * samplesize(2 Byte)
  return num_frames * channels * 2;
}

unsigned Config::get_period_time() const {
  // how long a period takes in us
  unsigned period_time;
  snd_pcm_hw_params_get_period_time(params, &period_time, 0);
  return period_time;
}

unsigned Config::get_period_frames() const {
  snd_pcm_uframes_t num_frames;
  // number of frames per period
  snd_pcm_hw_params_get_period_size(params, &num_frames, 0);
  return num_frames;
}