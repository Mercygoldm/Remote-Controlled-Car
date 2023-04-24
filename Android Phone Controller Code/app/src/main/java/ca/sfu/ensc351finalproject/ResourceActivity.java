package ca.sfu.ensc351finalproject;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.TextPaint;
import android.text.method.LinkMovementMethod;
import android.text.style.ClickableSpan;
import android.view.View;
import android.widget.TextView;

public class ResourceActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_resource);
        setUpToolBar();

        AppDetails();
        AuthorDetails();
        ResourcesDetails();
    }

    private void setUpToolBar() {
        setTitle("Resources");
        ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Resources");
    }

    private void AppDetails() {
        TextView appDetails = findViewById(R.id.txtViewAppDetails);
        String app_details = "Car Chasi is an app created to control a car chasi." +
                " This app connects with the chasi via bluetooth through a HC-06 bluetooth module";

        appDetails.setText(app_details);
    }

    private void AuthorDetails() {
        TextView authorDetails = findViewById(R.id.txtViewAuthorDetails);

        String link = "Car chasi is written by Aneet, Mercygold, Nasim and Ruth " +
                "who are four third year Engineering students at Simon Fraser University" +
                ". Taking ENSC 351 taught by Dr.Brian Fraser. " +
                "Here is a link to the course website: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/ensc351/home";

        String uri = "https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/ensc351/home";
        ConvertStringToLink(link,authorDetails,uri,link.length()-61,link.length());
    }

    private void ResourcesDetails() {
        TextView resourceDetails = findViewById(R.id.txtViewResources);
        String link1 = "https://www.electroniclinic.com/android-app-development-to-control-arduino-over-bluetooth-using-android-studio/";
        String link2 = "https://mixkit.co/free-sound-effects/car/";
        String link3 = "https://www.pinterest.ca/pin/780530179154197780/";
        String link4 = "https://www.pinterest.ca/pin/709598485047880334/";
        String link5 = "https://www.google.com/imgres?imgurl=https://www.franklinartglass.com/wp-content/uploads/0052-0114_-1-300x300.jpg&imgrefurl=https://www.franklinartglass.com/product/wissmach-silver-yellow-light-blue-gold-pink-and-white-opalescent-glass-35d/&tbnid=yT7--QG1csdFtM&vet=1&docid=fm0FIk05uDyPSM&w=300&h=300&hl=en-CA&source=sh/x/im";



        String message = "THE BLUETOOTH CODE INSPIRATION:-\n" + link1 + "\n\nTHE APP SOUND:\n" + link2 +"\n\nTHE BACKGROUND IMAGES\n" +
                link3 + "\n\n" + link4 + "\n\n" + link5;

        ConvertStringToLinkResource(message,resourceDetails);
    }

    private void ConvertStringToLinkResource(String link,TextView resourceDetails){
        String link1 = "https://www.electroniclinic.com/android-app-development-to-control-arduino-over-bluetooth-using-android-studio/";
        String link2 = "https://mixkit.co/free-sound-effects/car/";
        String link3 = "https://www.pinterest.ca/pin/780530179154197780/";
        String link4 = "https://www.pinterest.ca/pin/709598485047880334/";
        String link5 = "https://www.google.com/imgres?imgurl=https://www.franklinartglass.com/wp-content/uploads/0052-0114_-1-300x300.jpg&imgrefurl=https://www.franklinartglass.com/product/wissmach-silver-yellow-light-blue-gold-pink-and-white-opalescent-glass-35d/&tbnid=yT7--QG1csdFtM&vet=1&docid=fm0FIk05uDyPSM&w=300&h=300&hl=en-CA&source=sh/x/im";

        SpannableString links = new SpannableString(link);

        links.setSpan(linkSetUp(link1),33,146, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        links.setSpan(linkSetUp(link2),161,202, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        links.setSpan(linkSetUp(link3),225,275, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        links.setSpan(linkSetUp(link4),276,324, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        links.setSpan(linkSetUp(link5),325,link.length()-1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

        resourceDetails.setText(links);
        resourceDetails.setMovementMethod(LinkMovementMethod.getInstance());
    }

    private void ConvertStringToLink(String link,TextView resourceDetails,String uri, int startIndex, int endIndex){
        SpannableString links = new SpannableString(link);

        links.setSpan(linkSetUp(uri),startIndex,endIndex, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

        resourceDetails.setText(links);
        resourceDetails.setMovementMethod(LinkMovementMethod.getInstance());
    }

    private ClickableSpan linkSetUp(String link){
        return  new ClickableSpan() {
            @Override
            public void onClick(@NonNull View widget) {
                Uri courseWebsite = Uri.parse(link);
                Intent intent = new Intent(Intent.ACTION_VIEW,courseWebsite);
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(intent);
            }

            @Override
            public void updateDrawState(@NonNull TextPaint wordColour){
                wordColour.setColor(Color.BLUE);
                wordColour.setUnderlineText(true);
            }
        };
    }

    public static Intent makeResourceActivityIntent(MainActivity mainActivity) {
        return new Intent(mainActivity, ResourceActivity.class);
    }
}