package com.example.androidex;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;


public class MainActivity extends Activity {
    Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (msg.what == 0) {
                mBackText.setText("BackValue : " + msg.arg1);
            }
        }
    };

    class BackThread extends Thread {
        int mBackValue = 0;
        Handler sHandler;

        BackThread(Handler handler) {
            sHandler = handler;
        }

        public void run() {
            while (true) {
                mBackValue++;
                Message msg = Message.obtain();
                msg.what = 0;
                msg.arg1 = mBackValue;
                sHandler.sendMessage(msg);
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    ;
                }
            }
        }
    }

    LinearLayout linear;
    TextView mMainText;
    TextView mBackText;
    int mMainValue = 0;
    OnClickListener ltn;
    BackThread mThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        linear = (LinearLayout) findViewById(R.id.container);

        mMainText = (TextView) findViewById(R.id.mainvalue);
        mBackText = (TextView) findViewById(R.id.backvalue);

        Button btn = (Button) findViewById(R.id.increase);
        ltn = new OnClickListener() {
            public void onClick(View v) {
                mMainValue++;
                mMainText.setText("MainValue : " + mMainValue);
            }
        };
        btn.setOnClickListener(ltn);

        mThread = new BackThread(mHandler);
        mThread.setDaemon(true);
        mThread.start();
    }

}
