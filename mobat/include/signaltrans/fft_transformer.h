
#ifndef FFT_TRANSFORMER_H
#define FFT_TRANSFORMER_H

#include "buffer_collection.hpp"

#include <fftw3.h>
#include <iostream>
#include <cmath>

#include <map>

#include <vector>

#include <atomic>
#include <thread>

class FftTransformer {
	private:

    volatile bool keep_threads_alive_;
		unsigned int fft_window_size_;

		float normalization_value_;
		bool found_frequency_;

		std::array<fftw_plan, 4>  fftw_execution_plans_;

		int* input_stream_;

		std::array<unsigned int,4> start_sample_;
		std::array<unsigned int,4> end_sample_;

		buffer_collection audio_buffers_;

		unsigned int fft_frame_size_;
		std::array<double*,4> fft_in_;
		std::array<fftw_complex*,4> fft_result_;
		double* accumulated_result_;

		double* window_;

		std::array<std::map<unsigned, double>, 4> frequency_sums_;
		std::array<std::vector<double>, 4> average_smoothing_result_vectors_;

		std::array<std::shared_ptr<std::thread>, 4> fft_threads_;

		unsigned stabilization_counter_;

		unsigned int fft_frame_count_;

		std::vector<unsigned> listening_to_those_frequencies;

		std::array<std::atomic<bool>, 4> allow_fft_;
		std::array<std::atomic<bool>, 4> ffts_performed_;

		unsigned ints_per_channel_;
		unsigned signal_half_chunk_;



		void createHammingWindow();
		void createHannWindow();
		void createBlackmannHarrisWindow();



		void initializeFFTThreads();
        void performFFTOnCertainChannel(unsigned channel_iterator);

        void smoothResults(unsigned int channel_idx);

        // loads configurable parameters
		void loadFFTParameters();

		void copyChannelFFTResults( unsigned const channel_idx, std::vector<unsigned> const& observed_frequencies );

		//configurable parameters
		float normalization_range_lower_limit_;
		float normalization_range_upper_limit_;
		unsigned ffts_per_frame_;
		unsigned window_size_;
		unsigned num_chunks_;
		unsigned num_smooth_average_samples_;
		float frequency_slice_halfsize_;

		float audio_device_sampling_rate_;

	public:
		FftTransformer(unsigned int fft_frame_size);
		~FftTransformer();

    void shutdown();

		void resetSampleCounters(unsigned );
		void clearCachedFFTResults(unsigned);

		void initializeExecutionPlan();

		void setListenedFrequencies(std::vector<unsigned> const& listening_to_frequencies);

		void performFFTOnChannels(buffer_collection const& buffers, unsigned window_size, unsigned signal_chunk);

		void setFFTInput( unsigned int offset, unsigned int channel_num);
		unsigned performFFT(unsigned int channel_num );

		void setAnalyzationRange(unsigned int start_sample,
								   unsigned int end_sample,
								   unsigned int channel_num);
	
		std::map<unsigned, std::array<std::vector<double>,4> > signal_results_per_frequency_;

		//frequency, 4 channels, for each offset a value of fft sums
		std::map<unsigned, std::array<std::map<unsigned, double>,4>> fft_cached_results_per_frequency_;

		void resetThreadPerformedSignals();


};

#endif
