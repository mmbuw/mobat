#ifndef RING_BUFFER_H
#define RING_BUFFER_H


template <class DATA_TYPE>
class Ring_Buffer {

	public:
		Ring_Buffer(unsigned buffer_size);
		~Ring_Buffer();

		//reads num_samples from the current tail of the ringbuffer into the provided Buffer
		void Read_Samples(unsigned const num_samples, 
						  DATA_TYPE* buffer_to_write) const;

		void Write_Samples(unsigned const num_samples,
						  DATA_TYPE* buffer_to_read);

		//pushes one sample at the current head of the ringbuffer
		void Push_Sample(DATA_TYPE* buffer_to_read);

		void Reset_Buffer(unsigned new_buffer_size);

		unsigned Get_Capacity() const;

	private:
		unsigned capacity_;
		unsigned num_filled_elements_;

		DATA_TYPE* ring_data_;

		bool is_allocated_;

		unsigned head_;
		unsigned tail_;

};

template class Ring_Buffer<double>;

#endif