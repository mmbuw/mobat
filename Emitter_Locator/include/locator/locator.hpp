#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include <mutex>
#include <SFML/Graphics.hpp>

#include <map>

#include "microphone.hpp"
#include "recorder.hpp"
#include "tdoator.hpp"
#include "Signal_Analyzer.h"
#include "buffer_collection.hpp"

class Locator {
 public:
    Locator(unsigned int num_mics);

     std::map<unsigned, std::pair<unsigned, glm::vec2> > load_position() const;
     glm::vec4 const load_toas() const;
     std::array<std::vector<double>,4> const load_signal_vis_samples() const;
     std::array<unsigned, 4> const load_recognized_vis_sample_positions() const;

     void record_position();

     void shut_down();

private:

    mutable std::mutex position_mutex;
    mutable std::mutex toas_mutex;
    mutable std::mutex signal_vis_samples_mutex;
    mutable std::mutex recognized_vis_sample_pos_mutex;

    bool shutdown;
    std::map<unsigned, std::pair<unsigned, glm::vec2> > located_positions;
    std::map<unsigned, std::pair<unsigned, glm::vec2> > cached_located_positions;

    glm::vec4 toas;
    glm::vec4 cached_toas;

    std::array<unsigned, 4> recognized_vis_sample_pos;
    std::array<unsigned, 4> cached_recognized_vis_sample_pos;

    std::array<std::vector<double>,4> signal_vis_samples;
    std::array<std::vector<double>,4> cached_signal_vis_samples;
    
    Recorder recorder;
    buffer_collection collector;

    TDOAtor locator;
    Signal_Analyzer signal_analyzer;

 };

 #endif //LOCATOR_HPP