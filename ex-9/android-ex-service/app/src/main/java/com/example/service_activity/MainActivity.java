package com.example.service_activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button b1 = (Button) findViewById(R.id.button1);
        Button b2 = (Button) findViewById(R.id.button2);

        b1.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                Log.d("test", "Click Start Button");
                Intent intent = new Intent(
                        getApplicationContext(),
                        MyService.class);
                startService(intent);
            }
        });

        b2.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                Log.d("test", "Click End Button");
                Intent intent = new Intent(
                        getApplicationContext(),
                        MyService.class);
                stopService(intent);
            }
        });
    }
}
