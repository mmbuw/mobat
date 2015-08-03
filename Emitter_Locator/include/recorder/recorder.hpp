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

  //block generated copy operations 
  Recorder(Recorder const&) = delete;
  Recorder& operator=(Recorder const&) = delete;

  // bufferlength in bytes
  std::size_t bufferBytes() const;
  // returns bufferhandle
  uint8_t* buffer();

  // name of device used for recording
  std::string const& deviceName() const;
  // get used configuration
  Config const& config() const;
  // do new recording to buffer
  void record();
  
  // trigger new recording in recordingLoop
  void requestRecording();
  // check if new recording was created since last request
  bool newRecording() const;
  // loop to record when requested
  void recordingLoop();
  // stop recording loop
  void shutdown();

  static void outputCards();
  // helper methods to get avaible devices
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
