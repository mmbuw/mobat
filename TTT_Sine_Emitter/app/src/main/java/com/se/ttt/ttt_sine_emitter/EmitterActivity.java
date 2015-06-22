package com.se.ttt.ttt_sine_emitter;

import android.app.Activity;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.media.*;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

//initial sound sample code from:
//http://stackoverflow.com/questions/2413426/playing-an-arbitrary-tone-with-android

public class EmitterActivity extends Activity {
    //private final int durationInSeconds_ = 3; // seconds
    private final int currentSampleRate_ = 96000;

    private double sample[] = null; //= new double[currentNumSamples_];
    private double currentFrequencyOfTone_ = 18000; // hz

    private int samples_per_millisecond_ = 0;

    private boolean is_playing_ = false;

    private String[] sineLengthString_  = {"1000"};
    private String[] pauseLengthString_ = {"1000"};


    private byte generatedSnd[] = null;//new byte[2 * currentNumSamples_];

    private AudioTrack audioTrack = null;

    private SeekBar freqChangeSeekbar;
    private TextView freqTextView;
    private TextView statusLabel;
    //private Toast frequencyToast = null;

    Handler handler = new Handler();


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_emitter);


//        frequencyToast = Toast.makeText(this, "", Toast.LENGTH_SHORT);


        freqChangeSeekbar = (SeekBar) findViewById(R.id.FrequencySineWaveSeekBar);
        freqTextView = (TextView) findViewById(R.id.FrequencyTextView);
        statusLabel = (TextView) findViewById(R.id.status_label);

        freqChangeSeekbar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub

            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,
                                          boolean fromUser) {
                // TODO Auto-generated method stub
                // TODO Auto-generated method stub
                if(audioTrack != null) {

                    System.out.println(seekBar.getProgress());
                    currentFrequencyOfTone_ = seekBar.getProgress() * 100.0;

                    freqTextView.setText(currentFrequencyOfTone_ + " hz");
                } else {
                    //Toast.makeText(getBaseContext(),"Audiotrack is null", Toast.LENGTH_SHORT);
                }

            }
        });


        //initialize spinners
        Spinner sound_length_spinner = (Spinner) findViewById(R.id.Sound_Length_Spinner);
        String[] availableSoundLengths={"1", "2", "5", "10","20","50","100","200","300","400","500",
                                        "600","700", "800", "900", "1000","10000"};

        ArrayAdapter<String> sound_length_spinner_adapter
                = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item,
                                           availableSoundLengths);

        sound_length_spinner.setAdapter(sound_length_spinner_adapter);


        sound_length_spinner.setOnItemSelectedListener(
                new SimpleValueUpdaterItemSelectListener(sineLengthString_) );

        Spinner pause_length_spinner = (Spinner) findViewById(R.id.Pause_Length_Spinner);
        String[] availablePauseLengths={"5", "10", "20", "50", "100","200","300","400","500","600","700", "800", "900",
                                        "1000","1500", "2000", "10000"};
        ArrayAdapter<String> pause_length_spinner_adapter
                = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item,
                availablePauseLengths);

        pause_length_spinner.setAdapter(pause_length_spinner_adapter);

        pause_length_spinner.setOnItemSelectedListener(
                new SimpleValueUpdaterItemSelectListener(pauseLengthString_));


        Button Sine_Setting_Application_Button
                = (Button)findViewById(R.id.sine_setting_apply_button);
        Sine_Setting_Application_Button.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                /*Toast.makeText(getBaseContext(),
                        "SineLength  : " + sineLengthString_[0] + "\n" +
                                "PauseLength : " + pauseLengthString_[0],
                        Toast.LENGTH_SHORT).show();
                */

                if (audioTrack != null) {
                    audioTrack.stop();
                    audioTrack.flush();
                    audioTrack.release();
                }

                genTone(currentFrequencyOfTone_, currentSampleRate_);

                writeSound();
            }
        });

        Button Play_Button
                = (Button)findViewById(R.id.play_button);
        Play_Button.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                is_playing_ = true;
                playSound();
            }
        });

        Button Pause_Button
                = (Button)findViewById(R.id.pause_button);
        Pause_Button.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                is_playing_ = false;
                stopSound();
            }
        });

        freqTextView.setText(currentFrequencyOfTone_ + " hz");
        genTone(currentFrequencyOfTone_, currentSampleRate_);

        playSound();
    }

    @Override
    protected void onResume() {
        super.onResume();

        if(audioTrack != null) {
            //audioTrack.stop();
           // audioTrack.flush();
           // audioTrack.release();

            //genTone(currentFrequencyOfTone_, currentSampleRate_);

           // playSound();
        }
    }

    void genTone(double freqOfTone, double sampleRate){

        samples_per_millisecond_ = (int)Math.floor( currentSampleRate_/1000.0 );

        int num_sine_samples = Integer.parseInt(sineLengthString_[0]) * samples_per_millisecond_;
        int num_pause_samples = Integer.parseInt(pauseLengthString_[0]) * samples_per_millisecond_;
        int total_num_samples = num_sine_samples + num_pause_samples;

        System.out.println("Total num samples: " + total_num_samples + "\n");
        System.out.println("Duration Sine: " + num_sine_samples/(float)currentSampleRate_ +" seconds");
        System.out.println("Duration Silence: " + num_pause_samples/(float)currentSampleRate_ +" seconds");
        System.out.println("Total duration: " + total_num_samples/(float)currentSampleRate_ +" seconds");

        sample = new double[total_num_samples];

        int leveling_sample_threshold = 30;
        // fill out the array
        for (int i = 0; i < num_sine_samples; ++i) {
            double amplitude_scaling = 1.0;
            if(i < leveling_sample_threshold)
                amplitude_scaling = (1.0/leveling_sample_threshold)*i;
            else if(i > num_sine_samples-(leveling_sample_threshold+1))
                amplitude_scaling = ( (num_sine_samples) - i ) * (1.0/leveling_sample_threshold);

            sample[i] = Math.sin(2 * Math.PI * i / (sampleRate/freqOfTone)) * amplitude_scaling;
        }

        for (int i = num_sine_samples+1; i < total_num_samples; ++i) {
            sample[i] = 0.0;
        }

        // convert to 16 bit pcm sound array
        // assumes the sample buffer is normalised.

        generatedSnd = new byte[2 * total_num_samples];

        int idx = 0;
        for (final double dVal : sample) {
            // scale to maximum amplitude
            final short val = (short) ((dVal * 32767));
            // in 16 bit wav PCM, first byte is the low order byte
            generatedSnd[idx++] = (byte) (val & 0x00ff);
            generatedSnd[idx++] = (byte) ((val & 0xff00) >>> 8);

        }
    }

    void updateStatusLabel() {
        String status = "";
        status += "Sine Frequency: " + currentFrequencyOfTone_ + "\n";
        status += "Sine Length   :  " + sineLengthString_[0]  + "\n";
        status += "Pause Length  : " + pauseLengthString_[0] + "\n";

        if(is_playing_) {
            status += "Current status: Playing\n";
        } else {
            status += "Current status: Stopped\n";
        }
        statusLabel.setText(status);
    };

    void writeSound() {

        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                currentSampleRate_, AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_16BIT, generatedSnd.length,
                AudioTrack.MODE_STATIC);
        audioTrack.write(generatedSnd, 0, generatedSnd.length);

        updateStatusLabel();
    }

    void stopSound() {


        updateStatusLabel();
    }

    void playSound(){
        /*final AudioTrack*/

        if( null == audioTrack) {
            writeSound();
        }

        stopSound();
        audioTrack.setLoopPoints(0,  generatedSnd.length/2, -1);
        audioTrack.play();

        updateStatusLabel();
    }
}
