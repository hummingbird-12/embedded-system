package com.example.hangman;

import java.util.List;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class GameListAdapter extends ArrayAdapter<String> {
	private final Activity context;
	private final List<String> words;
	private final List<Integer> scores;
	
	public GameListAdapter(Activity context, List<String> words, List<Integer> scores) {
		super(context, R.layout.game_list_entry, words);
		
		this.context = context;
		this.words = words;
		this.scores = scores;
	}
	
	public View getView(int position, View view, ViewGroup parent) {		
		LayoutInflater inflater = context.getLayoutInflater();
		View rowView = inflater.inflate(R.layout.game_list_entry, null, true);
		
		TextView wordText = (TextView) rowView.findViewById(R.id.game_word);
		TextView scoreText = (TextView) rowView.findViewById(R.id.game_score);
		
		wordText.setText(words.get(position));
		scoreText.setText(scores.get(position) != -1 ? scores.get(position).toString() : "");
		
		return rowView;
	}
}
