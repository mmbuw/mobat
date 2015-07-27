#ifndef RECORDER_HPP
#define RECORDER_HPP

#include "config.hpp"
#include "device.hpp"
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>
#include <vector>

class Recorder {
 public:
  Recorder(unsigned chan = 2, std::size_t frames = 44100, std::size_t recording_time = 2000000);
  ~Recorder();

  static void outputCards();
  std::size_t bufferBytes() const;
  uint8_t* buffer();

  std::string const& deviceName() const;
  Config& config();
  void record();
  bool newRecording() const;
  void requestRecording();

  void recordingLoop();
  void shutdown();

  static std::vector<std::string> getPcms();
  static std::vector<std::string> getSupportingDevices(std::vector<std::string> const&, Config const&, snd_pcm_stream_t);
 private:

  Config config_;
  device device_;
  std::size_t recording_time_;
  std::size_t buffer_length_;
  bool new_recording_;
  bool shutdown_;
  // use char array for easy iteration over data
  uint8_t* buffer_;
};

#endif //RECORDER_HPP 
