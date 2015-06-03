package com.se.ttt.ttt_sine_emitter;

import android.view.View;
import android.widget.AdapterView;
import android.widget.Toast;

/**
 * Created by root on 03.06.15.
 */
public class SimpleValueUpdaterItemSelectListener implements AdapterView.OnItemSelectedListener {

    public String[] value_string_reference_;

    public SimpleValueUpdaterItemSelectListener(String[] str_to_reference) {
        value_string_reference_ = str_to_reference;
    }

    public void onItemSelected(AdapterView<?> parent, View view, int pos,long id) {
        Toast.makeText(parent.getContext(),
                "OnItemSelectedListener : " + parent.getItemAtPosition(pos).toString(),
                Toast.LENGTH_SHORT).show();

        value_string_reference_[0] = parent.getItemAtPosition(pos).toString();
    }

    @Override
    public void onNothingSelected(AdapterView<?> arg0) {
        // TODO Auto-generated method stub
    }
}
