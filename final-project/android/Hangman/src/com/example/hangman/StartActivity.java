package com.example.hangman;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class StartActivity extends Activity {

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.activity_start);
	    
	    OnClickListener listener = new OnClickListener() {
	    	public void onClick(View v) {
	    		Intent intent = new Intent(StartActivity.this, MainActivity.class);
	    		startActivity(intent);
	    	}
	    };
	    
	    Button startBtn = (Button) findViewById(R.id.start_btn);
	    startBtn.setOnClickListener(listener);
	}

}
