#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include <mutex>
#include <SFML/Graphics.hpp>

#include <map>

#include "microphone.hpp"
#include "recorder.hpp"
#include "tdoator.hpp"
#include "signal_analyzer.h"
#include "buffer_collection.hpp"

class Locator {
public:
    Locator(unsigned int num_mics);

    std::map<unsigned, std::pair<unsigned, glm::vec2> > loadPosition() const;
    glm::vec4 const loadTOAs() const;
    std::array<std::vector<double>,4> const loadSignalVisSamples() const;
    std::array<unsigned, 4> const loadRecognizedVisSamplePositions() const;
    std::map<unsigned, std::array<std::vector<unsigned> ,4> > const loadPeakSamples() const;

    void setFrequenciesToRecord(std::vector<unsigned> const& frequencies_to_find);

    void recordPosition();

    void shutdown();

     friend int main(int argc, char** argv);

private:

    mutable std::mutex position_mutex;
    mutable std::mutex toas_mutex;
    mutable std::mutex signal_vis_samples_mutex;
    mutable std::mutex recognized_vis_sample_pos_mutex;
    mutable std::mutex peak_sample_indices_mutex;

    std::mutex frequency_to_record_setter_mutex;

    std::map<unsigned, std::pair<unsigned, glm::vec2> > located_positions;
    std::map<unsigned, std::pair<unsigned, glm::vec2> > cached_located_positions;

    std::array<unsigned, 4> recognized_vis_sample_pos;
    std::array<unsigned, 4> cached_recognized_vis_sample_pos;

    std::array<std::vector<double>,4> signal_vis_samples;
    std::array<std::vector<double>,4> cached_signal_vis_samples;

    std::map<unsigned, std::array<std::vector<unsigned> ,4> > peak_sample_indices_;
    std::map<unsigned, std::array<std::vector<unsigned> ,4> > cached_peak_sample_indices_;
    
    std::vector<unsigned> frequencies_to_locate;

    std::map<unsigned, std::array<std::pair<unsigned, glm::vec2>, 20> > cached_positions;

    glm::vec4 toas_;
    glm::vec4 cached_toas_;

    bool shutdown_;
    Recorder recorder_;
    buffer_collection collector_;
    SignalAnalyzer signal_analyzer_;
    TDOAtor tdoator_;

    unsigned locator_frame_counter_;
    unsigned current_signal_chunk_;
 };

 #endif //LOCATOR_HPP
