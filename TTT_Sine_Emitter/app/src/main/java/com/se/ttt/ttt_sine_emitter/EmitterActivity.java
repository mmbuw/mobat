package com.se.ttt.ttt_sine_emitter;

import android.app.Activity;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.media.*;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

//initial sound sample code from:
//http://stackoverflow.com/questions/2413426/playing-an-arbitrary-tone-with-android

public class EmitterActivity extends Activity {
    private final int durationInSeconds_ = 1; // seconds
    private final int currentSampleRate_ = 96000;
    private final int currentNumSamples_ = durationInSeconds_ * currentSampleRate_;
    private final double sample[] = new double[currentNumSamples_];
    private double currentFrequencyOfTone_ = 18000; // hz

    private final byte generatedSnd[] = new byte[2 * currentNumSamples_];

    private AudioTrack audioTrack = null;

    private SeekBar freqChangeSeekbar;
    private TextView freqTextView;
    //private Toast frequencyToast = null;

    Handler handler = new Handler();



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_emitter);

        genTone(0, currentSampleRate_);

        playSound();

//        frequencyToast = Toast.makeText(this, "", Toast.LENGTH_SHORT);


        freqChangeSeekbar = (SeekBar) findViewById(R.id.FrequencySineWaveSeekBar);
        freqTextView = (TextView) findViewById(R.id.FrequencyTextView);

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
                    audioTrack.stop();
                    audioTrack.flush();
                    audioTrack.release();
                    System.out.println(seekBar.getProgress());
                    currentFrequencyOfTone_ = seekBar.getProgress() * 100.0;
                    genTone(currentFrequencyOfTone_, currentSampleRate_);

                    playSound();

                    //frequencyToast.setText(currentFrequency + " hz");
                    //frequencyToast.show();
                    freqTextView.setText(currentFrequencyOfTone_ + " hz");
                }

            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();

        if(audioTrack != null) {
            audioTrack.stop();
            audioTrack.flush();
            audioTrack.release();

            genTone(currentFrequencyOfTone_, currentSampleRate_);

            playSound();
        }
    }

    void genTone(double freqOfTone, double sampleRate){
        // fill out the array
        for (int i = 0; i < currentNumSamples_; ++i) {
            sample[i] = Math.sin(2 * Math.PI * i / (sampleRate/freqOfTone));
        }

        // convert to 16 bit pcm sound array
        // assumes the sample buffer is normalised.
        int idx = 0;
        for (final double dVal : sample) {
            // scale to maximum amplitude
            final short val = (short) ((dVal * 32767));
            // in 16 bit wav PCM, first byte is the low order byte
            generatedSnd[idx++] = (byte) (val & 0x00ff);
            generatedSnd[idx++] = (byte) ((val & 0xff00) >>> 8);

        }
    }

    void playSound(){
        /*final AudioTrack*/
                audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                currentSampleRate_, AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_16BIT, generatedSnd.length,
                AudioTrack.MODE_STATIC);
        audioTrack.write(generatedSnd, 0, generatedSnd.length);

        audioTrack.stop();
        audioTrack.setLoopPoints(0,  generatedSnd.length/4, -1);
        audioTrack.play();
    }
}
