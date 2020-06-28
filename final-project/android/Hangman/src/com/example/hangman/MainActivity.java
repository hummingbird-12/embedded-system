package com.example.hangman;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.ListView;

public class MainActivity extends Activity {
	public native int openDevice();

	public native void startHangman(int fd);

	public native void closeDevice(int fd);

	private ListView gameList;
	private GameListAdapter adapter;

	private List<String> words = new ArrayList<String>();
	private List<Integer> scores = new ArrayList<Integer>();

	private Handler mainHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (msg.what == 0 && msg.arg1 == 1) {
				exitHangman();
			}
		}
	};

	class InputThread extends Thread {
		Handler handler;

		InputThread(Handler _handler) {
			handler = _handler;
		}

		public void run() {
			int fd = openDevice();
			startHangman(fd);
			closeDevice(fd);

			Message msg = Message.obtain();
			msg.what = 0;
			msg.arg1 = 1;
			handler.sendMessage(msg);
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		 System.loadLibrary("hangman");

		adapter = new GameListAdapter(this, words, scores);

		gameList = (ListView) findViewById(R.id.game_list);
		gameList.setAdapter(adapter);

		this.addGame("Hello");

		InputThread iThread = new InputThread(mainHandler);
		iThread.setDaemon(true);
		iThread.start();
	}

	private void addGame(String word) {
		Log.i("HANGMAN", "[addGame] word: " + word);

		this.words.add(word);
		this.scores.add(-1);

		adapter.notifyDataSetChanged();
	}

	private void updatelastGame(String word) {
		Log.i("HANGMAN", "[updatelastGame] word: " + word);

		this.words.set(this.words.size() - 1, word);

		adapter.notifyDataSetChanged();
	}

	private void updateGameList(String word, int score) {
		Log.i("HANGMAN", "[updateGameList] word: " + word + "  score: "
				+ String.valueOf(score));

		this.words.set(this.words.size() - 1, word);
		this.scores.set(this.scores.size() - 1, score);

		adapter.notifyDataSetChanged();

		this.addGame("");
	}

	private void exitHangman() {
		Log.i("HANGMAN", "[exitHangman] exiting");

		finish();
	}
}
