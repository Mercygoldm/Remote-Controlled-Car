package ca.sfu.ensc351finalproject;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

public class HelpScreenActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_help_screen);
        setUpToolBar();

        TextView help = findViewById(R.id.instructions);
        help.setText("Forward: Press Forward to move the car forward\n\n" +
                "Back: Press backwards to move the car back\n\n" +
                "Left: Press left to turn the car to the left\n\n" +
                "Right: Press right to move the car to the right\n\n" +
                "SpeedUp: Press speed up to accelerate the car\n\n" +
                "SpeedDown: Press speed down to decelerate the car\n\n");

    }
    private void setUpToolBar() {
        setTitle("Help");
        ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Help");
    }

    public static Intent makeHelpScreenIntent(Context context) {
        return new Intent(context, HelpScreenActivity.class);
    }

    @Override
    public void onBackPressed() {
        //super.onBackPressed();
        finish();
    }
}