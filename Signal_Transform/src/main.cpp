#include "../include/FFT_Transformer.h"

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <iostream>
#include <cmath>
#include <chrono>

unsigned int N = 2048;

int* load_WAV_to_buffer(std::string const& in_wav_file,
						SF_INFO& info) {

    SNDFILE *sf;

    //int num_channels;
    int num_items;

    int f,sr,c;
    //int i,j;
    
    /* Open the WAV file. */
    info.format = 0;
    sf = sf_open(in_wav_file.c_str(),SFM_READ,&info);
    if (sf == NULL)
        {
        printf("Failed to open the file.\n");
        exit(-1);
        }
    /* Print some of the info, and figure out how much data to read. */
    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
    printf("frames=%d\n",f);
    printf("samplerate=%d\n",sr);
    printf("channels=%d\n",c);
    num_items = f*c;
    printf("num_items=%d\n",num_items);
    /* Allocate space for the data to be read, then read it. */
    int *ampl_buf = (int *) malloc(num_items*sizeof(int));
    sf_read_int(sf,ampl_buf,num_items);
    sf_close(sf);

    return ampl_buf;
}


int main(int argc, char** argv) {

    if(argc < 2) {
    	std::cout << "Program Usage: program <filename.wav>\n\n";

    	return -1;
    }

    FFT_Transformer fft_transf(N);

    SF_INFO info;

    int *buf = load_WAV_to_buffer(argv[1], info);;

    int **buffer_collector = 0;

    buffer_collector 
        = (int **) malloc(1*sizeof(int*));


    fft_transf.initialize_execution_plan();

    buffer_collector[0] = buf;

    std::cout << "Before setting input\n";

std::chrono::system_clock::time_point before_fft = std::chrono::system_clock::now();
    for(unsigned int i = 0; i < 200000; ++i) {

        unsigned offset = 1 * i;
        if(offset > 200000)
            break;

        fft_transf.set_FFT_input(1, 
                                 info.frames,
                                 buffer_collector,
                                 offset);
        fft_transf.perform_FFT();

        fft_transf.print_FFT_result(i);

     
    } 
std::chrono::system_clock::time_point after_fft = std::chrono::system_clock::now();
std::cout << "fftw execution time: " <<
std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count() << "\n";
    std::cout << "After setting input\n";
 



    return 0;
}