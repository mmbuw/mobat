#ifndef RECORDER_HPP
#define RECORDER_HPP

#include "config.hpp"
#include "device.hpp"
// requires libasound2-dev
#include </usr/include/alsa/asoundlib.h>
#include <vector>

class Recorder {
 public:
  Recorder(unsigned chan = 2, unsigned frames = 44100, std::size_t recording_time = 2000000);
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
  // update writing_offset and recorded_bytes
  void increment_writer();

  // check if new recording was created since last buffer red
  bool newRecording() const;
  // number of new bytes recorded since last read, max is buffer_bytes_
  std::size_t recordedBytes() const;
  // loop to record
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
  std::size_t buffer_bytes_;
  // offset from buffer beginning in bytes
  std::size_t writing_offset_;
  // new bytes recorded since last request
  std::size_t recorded_bytes_;
  bool new_recording_;
  bool shutdown_;
  // use char array for easy iteration over data
  uint8_t* buffer_;
  uint8_t* output_buffer_;
};

#endif //RECORDER_HPP 
