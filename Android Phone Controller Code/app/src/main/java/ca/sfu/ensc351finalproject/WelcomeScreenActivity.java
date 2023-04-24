package ca.sfu.ensc351finalproject;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.widget.TextView;

public class WelcomeScreenActivity extends AppCompatActivity {
    MediaPlayer mediaPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_welcome_screen);
        setUpToolBar();

        findViewById(R.id.btnSkip).setOnClickListener(v->finish());
        playWelcomeScreenSound();
        displayTime();
    }

    private void displayTime() {
        TextView textView = findViewById(R.id.txtVAppName);

        textView.animate().setDuration(8500).withEndAction(()->finish()).start();
    }

    private void setUpToolBar() {
        setTitle("Welcome Screen");
        ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Welcome Screen");
    }

    private void playWelcomeScreenSound(){
        if(mediaPlayer == null){
            mediaPlayer = MediaPlayer.create(this,R.raw.sound_1_welcome_screen);
        }
        mediaPlayer.start();
    }

    public static Intent makeWelcomeIntent(Context context) {
        return new Intent(context, WelcomeScreenActivity.class);
    }

}