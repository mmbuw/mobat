
#ifndef FFT_TRANSFORMER_H
#define FFT_TRANSFORMER_H

#include "ring_buffer.h"
#include "buffer_collection.hpp"

#include <fftw3.h>
#include <iostream>
#include <cmath>

#include <map>

#include <vector>

#include <atomic>
#include <thread>

class FFT_Transformer {
	private:

		float normalization_value;
		bool found_freq_;

		std::array<fftw_plan, 4>  fftw_execution_plans_;
		unsigned int fft_window_size_;
		int* input_stream_;

		std::array<unsigned int,4> start_sample_;
		std::array<unsigned int,4> end_sample_;

		unsigned int audio_buffer_size_;
		unsigned int num_audio_buffers_;
		int** audio_buffers_;

		unsigned short fft_frame_size_;
		std::array<double*,4> fft_in_;
		std::array<fftw_complex*,4> fft_result_;
		double* accumulated_result_;

		double* window_;

		//double eighteen_khz_sum_;
		std::array<std::map<unsigned, double>, 4> frequency_sums_;

		std::array<std::shared_ptr<std::thread>, 4> fft_threads_;

		unsigned stabilization_counter_;

		unsigned int fft_frame_count_;

		std::vector<unsigned> listening_to_those_frequencies;

		std::array<std::atomic<bool>, 4> allow_fft_;
		std::array<std::atomic<bool>, 4> ffts_performed_;

		unsigned ints_per_channel_;
		unsigned signal_half_chunk_;
		unsigned window_size_;
		unsigned num_chunks_;

		void create_hamming_window();
		void create_hann_window();
		void create_blackmann_harris_window();

        void perform_FFT_on_certain_channel(unsigned channel_iterator);

	public:
		FFT_Transformer(unsigned short fft_frame_size);
		~FFT_Transformer();

		void reset_sample_counters(unsigned );
		void clear_cached_fft_results(unsigned);

		void initialize_execution_plan();

		void set_listened_frequencies(std::vector<unsigned> const& listening_to_frequencies);

		void perform_FFT_on_channels(buffer_collection const& buffers, unsigned window_size, unsigned signal_chunk);

		void set_FFT_input( unsigned int offset, unsigned int channel_num);
		unsigned perform_FFT(unsigned int channel_num );

		void set_analyzation_range(unsigned int start_sample,
								   unsigned int end_sample,
								   unsigned int channel_num);
	
		void set_FFT_buffers( unsigned int num_buffers,
							  unsigned int buffer_size,
							  buffer_collection const& signal_buffers);

		std::map<unsigned, std::array<std::vector<double>,4> > signal_results_per_frequency_;

		//frequency, 4 channels, for each offset a value of fft sums
		std::map<unsigned, std::array<std::map<unsigned, double>,4>> fft_cached_results_per_frequency_;

		void reset_thread_performed_signals();
};

#endif
