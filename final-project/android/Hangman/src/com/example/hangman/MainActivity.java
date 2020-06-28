package com.example.hangman;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.ListView;
import android.widget.Toast;

public class MainActivity extends Activity {
	public native int openDevice();

	public native void startHangman(int fd, Payload pl);

	public native void closeDevice(int fd);
	
	final int STATUS_EXIT = -1;
	final int STATUS_INPUT = 0;
	final int STATUS_GUESSED = 1;

	private ListView gameList;
	private GameListAdapter adapter;

	private List<String> words = new ArrayList<String>();
	private List<Integer> scores = new ArrayList<Integer>();
	
	private int maxScore = 0;

	/*
	 * Message handler
	 */
	private Handler mainHandler = new Handler() {
		public void handleMessage(Message msg) {
			Bundle bundle;
			String _word;
			int _score;
			switch (msg.what) {
			case STATUS_INPUT:
				bundle = msg.getData();
				_word = bundle.getString("word");
				updatelastGame(_word);
				
				break;
			case STATUS_GUESSED:
				bundle = msg.getData();
				_word = bundle.getString("word");
				_score = bundle.getInt("score");
				updateGameList(_word, _score);
				break;
			case STATUS_EXIT:
				exitHangman();
				break;
			default:
				break;
			}
		}
	};
	
	/*
	 * Payload between Module and App
	 */
	class Payload {
		String word;
		int status;
		int score;
	}

	/*
	 * Secondary thread to wait for Module's input
	 */
	class InputThread extends Thread {
		Handler handler;

		InputThread(Handler _handler) {
			handler = _handler;
		}

		public void run() {
			int fd = openDevice();
			
			while(true) {
				boolean exitGame = false;
				Payload pl = new Payload();
				Bundle bundle = new Bundle();
				Message msg = Message.obtain();
				
				startHangman(fd, pl);
				
				switch (pl.status) {
				case STATUS_INPUT:
					Log.i("HANGMAN", "[input] input: " + pl.word);
					
					bundle.putString("word", pl.word);
					
					msg.what = STATUS_INPUT;
					msg.setData(bundle);
					
					handler.sendMessage(msg);
					break;
				case STATUS_GUESSED:
					Log.i("HANGMAN", "[input] guessed: " + pl.word + "  score: " + String.valueOf(pl.score));
					
					bundle.putString("word", pl.word);
					bundle.putInt("score", pl.score);
					
					msg.what = STATUS_GUESSED;
					msg.setData(bundle);
					
					handler.sendMessage(msg);
					break;
				case STATUS_EXIT:
					Log.i("HANGMAN", "[input] exit");
					
					exitGame = true;
					break;
				default:
					break;
				}
				
				if (exitGame) {
					break;
				}
			}
			closeDevice(fd);
			
			Message msg = Message.obtain();
			msg.what = STATUS_EXIT;
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

		InputThread iThread = new InputThread(mainHandler);
		iThread.setDaemon(true);
		iThread.start();
		
		this.addGame("");
	}

	/*
	 * Add a new game to the list
	 */
	private void addGame(String word) {
		Log.i("HANGMAN", "[addGame] word: " + word);

		this.words.add(word);
		this.scores.add(-1);

		adapter.notifyDataSetChanged();
	}

	/*
	 * Update the last game
	 */
	private void updatelastGame(String word) {
		Log.i("HANGMAN", "[updatelastGame] word: " + word);

		this.words.set(this.words.size() - 1, word);

		adapter.notifyDataSetChanged();
	}

	/*
	 * Update last game list with score
	 */
	private void updateGameList(String word, int score) {
		Log.i("HANGMAN", "[updateGameList] word: " + word + "  score: "
				+ String.valueOf(score));

		this.words.set(this.words.size() - 1, word);
		this.scores.set(this.scores.size() - 1, score);

		adapter.notifyDataSetChanged();

		this.addGame("");
		maxScore = score;
	}

	/*
	 * Exit hangman game
	 */
	private void exitHangman() {
		Log.i("HANGMAN", "[exitHangman] exiting");
		
		int finalScore = Math.max(maxScore, 0);
		Toast.makeText(getApplicationContext(), "Your scored " + String.valueOf(finalScore) + " points", Toast.LENGTH_LONG).show();

		finish();
	}
}
