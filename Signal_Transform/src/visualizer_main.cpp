#include <fftw3.h>

#include <iostream>

#include <chrono>
#include <cmath>

#include <SFML/Graphics.hpp>

unsigned  N = 512;

unsigned currentFrequency_ = 400;
unsigned durationInSeconds_ = 10;
unsigned currentSampleRate_ = 96000;
unsigned currentNumSamples_ = durationInSeconds_ * currentSampleRate_;

unsigned windowWidth  = 1900;
unsigned windowHeight = 600;

double* sample;
unsigned char* generatedSnd;

double processingTimeMicroSeconds = 0.0;
void genTone(double freqOfTone, double sampleRate);

int main(int argc, char** argv) {

sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();
sf::Font arialFont;
arialFont.loadFromFile("../fonts/arial.ttf");
sf::RenderWindow window(fullScreenMode, "Fourier Vis", sf::Style::Fullscreen);
sf::RectangleShape fourierSampleRect_(sf::Vector2f(50,100));
sample = new double[currentNumSamples_];
generatedSnd = new unsigned char[2*currentNumSamples_];
genTone(currentFrequency_, currentSampleRate_);
double *in;

fftw_complex *out;
fftw_plan fftw_exec_plan;

in = (double*) fftw_malloc(sizeof(double) * N);
out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
fftw_exec_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
//fftw_exec_plan_reverse = fftw_plan_dft_c2r_1d(N, out, in, FFTW_MEASURE);
for(unsigned signal_it = 0; signal_it < N; ++signal_it) {
in[signal_it] = sample[signal_it];
}
std::chrono::system_clock::time_point before_fft = std::chrono::system_clock::now();
fftw_execute(fftw_exec_plan);
//fftw_execute(fftw_exec_plan_reverse);
std::chrono::system_clock::time_point after_fft = std::chrono::system_clock::now();
for(unsigned signal_it = 0; signal_it < N/2; ++signal_it) {
//std::cout << "In: " << in[signal_it]/float(N) << " Out: " << out[signal_it][0] << "\n";
}
double maxAmplitude = -1.0f;
for(unsigned signal_it = 0; signal_it < 100 - 1; ++signal_it) {
//std::cout << "Freq: " << (signal_it * currentSampleRate_)/N<<"\n";//_Ampli: "<<
double currentAmplitude = std::sqrt(out[signal_it][0]*out[signal_it][0] + out[signal_it][1]*out[signal_it][1]);
//std::cout << "In: " << in[signal_it]/float(N) << " Out: "
//<< currentAmplitude << "\n";
maxAmplitude = maxAmplitude > currentAmplitude ? maxAmplitude : currentAmplitude;
}
std::cout << "fftw execution time: " <<
std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count() << "\n";
processingTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count();


    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();



            if (event.type == sf::Event::KeyPressed) {

            	if(event.key.code == sf::Keyboard::Escape)
            		window.close();

            	if(event.key.code == sf::Keyboard::Up 
            		|| event.key.code == sf::Keyboard::Down
            		|| event.key.code == sf::Keyboard::Left 
            		|| event.key.code == sf::Keyboard::Right) {
	            	if (event.key.code == sf::Keyboard::Up) {
	            		currentFrequency_ += 1000;
	            	}
	            	if (event.key.code == sf::Keyboard::Down) {
	            		currentFrequency_ -= 1000;
	            	}
	            	if (event.key.code == sf::Keyboard::Left) {
	            		if (N > 2) {
	            			N /= 2;

	            			fftw_free(in);
	            			fftw_free(out);
							in = (double*) fftw_malloc(sizeof(double) * N);
							out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

	            			fftw_exec_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
	            		} 
	            	} 
	            	if (event.key.code == sf::Keyboard::Right) {

	            		if( N < 5000) {
	            			N *= 2;

	            			fftw_free(in);
	            			fftw_free(out);
							in = (double*) fftw_malloc(sizeof(double) * N);
							out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	            			fftw_exec_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
	            		} 
	            	}


	            	genTone(currentFrequency_, currentSampleRate_);
					for(unsigned signal_it = 0; signal_it < N; ++signal_it) {
						in[signal_it] = sample[signal_it];
					}
					 before_fft = std::chrono::system_clock::now();
					 fftw_execute(fftw_exec_plan);
					 after_fft = std::chrono::system_clock::now();
					 processingTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count();

	            }
            }
               
        }

        window.clear();


		for(unsigned signal_it = 0; signal_it < N/2-1; ++signal_it) {
			double ampl = std::sqrt(out[signal_it][0]*out[signal_it][0] + out[signal_it][1]*out[signal_it][1]) * windowHeight/maxAmplitude ;
			fourierSampleRect_.setSize( sf::Vector2f(windowWidth/(N/2-1), 
				std::sqrt(out[signal_it][0]*out[signal_it][0] + out[signal_it][1]*out[signal_it][1]) * windowHeight/maxAmplitude ) );

			fourierSampleRect_.setPosition(signal_it*(windowWidth/(N/2-1)),windowHeight-ampl);
			//std::cout<<std::sqrt(out[signal_it][0]*out[signal_it][0] + out[signal_it][1]*out[signal_it][1])<<"\n";
		    window.draw(fourierSampleRect_);




		}
        sf::Text frequencyLabel("0",arialFont);
        frequencyLabel.setCharacterSize(20);
        frequencyLabel.setPosition(0,windowHeight - frequencyLabel.getCharacterSize());

        if(frequencyLabel.getColor() == sf::Color::Red) {
             frequencyLabel.setColor(sf::Color::Green);  	
        } else {
       		frequencyLabel.setColor(sf::Color::Green);  	
        }



        sf::Text fourierWindowSizeLabel("",arialFont);
        	fourierWindowSizeLabel.setCharacterSize(20);
        sf::Text sineWaveFrequencyLabel("",arialFont);
        	sineWaveFrequencyLabel.setCharacterSize(20);
        sf::Text processingTimeLabel("", arialFont);
        	processingTimeLabel.setCharacterSize(20);

        fourierWindowSizeLabel.setPosition(0.0, 1.5*fourierWindowSizeLabel.getCharacterSize());
        fourierWindowSizeLabel.setColor(sf::Color::Red);
        fourierWindowSizeLabel.setString("Fourier-Window Size: " + std::to_string(N) );

       // sineWaveFrequencyLabel.move(windowWidth - 21*fourierWindowSizeLabel.getCharacterSize(),
        //							fourierWindowSizeLabel.getCharacterSize() );
        sineWaveFrequencyLabel.setColor(sf::Color::Red);   
        sineWaveFrequencyLabel.setString( "Sine Wave Freq (Hz): " + std::to_string( int(currentFrequency_) ));       

        processingTimeLabel.setPosition(0.0, 3*fourierWindowSizeLabel.getCharacterSize());
		processingTimeLabel.setColor(sf::Color::Red);
		processingTimeLabel.setString("Processing took " + std::to_string( int(processingTimeMicroSeconds) ) + " microseconds.");

        window.draw(fourierWindowSizeLabel);
        window.draw(sineWaveFrequencyLabel);
        window.draw(processingTimeLabel);

        double moved_width = 0.0;
		for(unsigned signal_it = 0; signal_it < N/2-1; ++signal_it) {
			frequencyLabel.move(windowWidth/(N/2-1),0);	

			moved_width += windowWidth/(N/2-1); 

			if(moved_width > 7 * fourierWindowSizeLabel.getCharacterSize()) {
				frequencyLabel.setString( std::to_string((signal_it * currentSampleRate_)/N ) );
		    	window.draw(frequencyLabel);
		       moved_width = 0.0;
			}


		}


        window.display();
    }

	fftw_destroy_plan(fftw_exec_plan);
	fftw_free(in); 
	fftw_free(out);

	delete sample;
	return 0;
}



void genTone(double freqOfTone, double sampleRate){
// fill out the array
for (unsigned int i = 0; i < currentNumSamples_; ++i) {
sample[i] = std::sin(2 * M_PI * i / (sampleRate/freqOfTone));
}
// convert to 16 bit pcm sound array
// assumes the sample buffer is normalised.
int idx = 0;
for (unsigned int sample_idx = 0; sample_idx < currentNumSamples_; ++sample_idx) {
// scale to maximum amplitude
double dVal = sample[sample_idx];
short val = (short) ((dVal * 32767));
// in 16 bit wav PCM, first byte is the low order byte
generatedSnd[idx++] = (unsigned char) (val & 0x00ff);
generatedSnd[idx++] = (unsigned char) ((val & 0xff00) >> 8);
}
}