#ifndef SIGNAL_ANALYZER_H
#define SIGNAL_ANALYZER_H

#include "FFT_Transformer.h"
#include "buffer_collection.hpp"

#include <array>

class Signal_Analyzer {

	private:
		unsigned short fft_window_size;
    	FFT_Transformer fft_transformer;

        std::map<unsigned, std::array<unsigned, 4> > vis_sample_pos_mapping;
    	std::map<unsigned, std::array<double, 4> > frequency_toas_mapping;
    	std::map<unsigned, bool>	is_frequency_toa_mapping_valid;

    public:
    	Signal_Analyzer();
    	~Signal_Analyzer();

    	/*
          calls fast fourier transformation for the input channels.
    	  then analyzes the frequencies it listens to for toas
    	  these toas are stored and can be retrieved by 
    	  "get_toas_for(unsigned const frequency)"
        */
    	void analyze(buffer_collection const&);

    	/*
          unregisters a frequency in order to not analyze it anymore
        */
    	void stop_listening_to(unsigned const frequency);
    	/*
          registers a frequency in order to analyze it
        */
    	void start_listening_to(unsigned const frequency);

    	/*
    	  retrieve the 4 toas for the specified frequency
    	*/
    	std::array<double, 4> get_toas_for(unsigned const frequency);


        /*
          returns the sample positions for the recognized start of the signal
          per channel
        */
        std::array<unsigned, 4> const
        get_vis_sample_pos_for(unsigned const frequency);


        /*
    	  returns the fourier transformed and accumulated samples for
    	  the specified frequency
    	*/
    	std::array<std::vector<double>,4> const 
    	get_signal_samples_for(unsigned const frequency);

    	/*
    	  stores the fourier transformed and accumulated samples 
    	  for the frequencies
    	*/
		std::map<unsigned, std::array<unsigned, 4> > 
		signal_detected_at_sample_per_frequency;
};

#endif