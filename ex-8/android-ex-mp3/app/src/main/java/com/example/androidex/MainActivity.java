package com.example.androidex;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;


public class MainActivity extends Activity {
    MediaPlayer mp1, mp2;
    Button btn, helloBtn, worldBtn;
    OnClickListener ltn, ltn1, ltn2;
    LinearLayout linear;
    TextView tv;//TextView
    int mp1_cnt = 0, mp2_cnt = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        linear = (LinearLayout) findViewById(R.id.container);
        //button find
        Button btn = (Button) findViewById(R.id.newactivity);
        helloBtn = (Button) findViewById(R.id.btn1);
        worldBtn = (Button) findViewById(R.id.btn2);
        //textview find
        tv = (TextView) findViewById(R.id.textView1);
        //mp3
        mp1 = MediaPlayer.create(this, R.raw.hello);
        mp2 = MediaPlayer.create(this, R.raw.world);

        //mp3_1 button listener
        ltn1 = new OnClickListener() {
            public void onClick(View v) {
                mp1_cnt++;
                tv.setText("Now, mp3_111111!!!!\n");
                if (mp1_cnt % 2 == 1)
                    mp1.start();
                else
                    mp1.pause();
            }
        };
        helloBtn.setOnClickListener(ltn1);
        //mp3_2 button listener
        ltn2 = new OnClickListener() {
            public void onClick(View v) {
                mp2_cnt++;
                tv.setText("Now, mp3_22222!!!!\n");
                if (mp2_cnt % 2 == 1)
                    mp2.start();
                else
                    mp2.pause();
            }
        };
        worldBtn.setOnClickListener(ltn2);
        //new activity listener
        ltn = new OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, MainActivity2.class);
                startActivity(intent);
            }
        };
        btn.setOnClickListener(ltn);
    }

}
