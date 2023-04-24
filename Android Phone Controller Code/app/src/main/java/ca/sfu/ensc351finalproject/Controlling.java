package ca.sfu.ensc351finalproject;

import android.Manifest;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.util.UUID;

import pub.devrel.easypermissions.EasyPermissions;

/*
 * Line 48 to line 144 is our code
 * The rest of this class was inspired by the code in this link:-
   https://www.electroniclinic.com/android-app-development-to-control-arduino-over-bluetooth-using-android-studio/
 */
public class Controlling extends AppCompatActivity {
    private static final String TAG = "BlueTest5-Controlling";
    private int mMaxChars = 50000;//Default//change this to string..........
    private UUID mDeviceUUID;
    private BluetoothSocket mBTSocket;
    private String newline = "\r\n";

    private boolean mIsUserInitiatedDisconnect = false;
    private boolean mIsBluetoothConnected = false;

    private Button mBtnDisconnect;
    private BluetoothDevice mDevice;

    final static String forward = "8";
    final static String backward = "1";
    final static String left = "4";
    final static String right = "2";
    final static String stop = "15";
    final static String speedUp = "7";
    final static String speedDown = "3";


    private ProgressDialog progressDialog;
    private final int LOCATION_PERMISSION_REQUEST = 101;

    public static final String[] BLUETOOTH_PERMISSIONS_S = {Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_controlling);
        setUpToolBar();

        Intent intent = getIntent();
        Bundle b = intent.getExtras();
        mDevice = b.getParcelable(MainActivity.DEVICE_EXTRA);
        mDeviceUUID = UUID.fromString(b.getString(MainActivity.DEVICE_UUID));
        mMaxChars = b.getInt(MainActivity.BUFFER_SIZE);

        Log.d(TAG, "Ready");
        findViewById(R.id.helpScreen).setOnClickListener(v -> launchHelpScreen());


        findViewById(R.id.up).setOnClickListener(v -> sendData(forward));
        findViewById(R.id.down).setOnClickListener(v -> sendData(backward));
        findViewById(R.id.left).setOnClickListener(v -> sendData(left));
        findViewById(R.id.right).setOnClickListener(v -> sendData(right));

        findViewById(R.id.stop).setOnClickListener(v -> sendData(stop));

        findViewById(R.id.volumeUp).setOnClickListener(v -> sendData(speedUp));
        findViewById(R.id.volumeDown).setOnClickListener(v -> sendData(speedDown));
    }

    private void sendData(String task) {
        try {
            byte[] data = (task + newline).getBytes();
            mBTSocket.getOutputStream().write(data);
            Toast.makeText(Controlling.this, "SENT", Toast.LENGTH_SHORT).show();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (!EasyPermissions.hasPermissions(this, BLUETOOTH_PERMISSIONS_S)) {
                EasyPermissions.requestPermissions(this, "message", LOCATION_PERMISSION_REQUEST, BLUETOOTH_PERMISSIONS_S);
            }
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(BLUETOOTH_PERMISSIONS_S, 1);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == LOCATION_PERMISSION_REQUEST) {
            if (!(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                new AlertDialog.Builder(this)
                        .setCancelable(false)
                        .setMessage("Location permission is required for this app.")
                        .setPositiveButton("Grant", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                checkPermissions();
                            }
                        })
                        .setNegativeButton("Deny", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                Controlling.this.finish();
                            }
                        }).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void setUpToolBar() {
        setTitle("Car Driver");
        ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Car Driver");
    }

    private void launchHelpScreen() {
        Intent intent = HelpScreenActivity.makeHelpScreenIntent(this);
        startActivity(intent);
    }

    @Override
    protected void onResume() {
        if (mBTSocket == null || !mIsBluetoothConnected) {
            new ConnectBT().execute();
        }
        Log.d(TAG, "Resumed");
        super.onResume();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    private class ConnectBT extends AsyncTask<Void, Void, Void> {
        private boolean mConnectSuccessful = true;

        @Override
        protected void onPreExecute() {
            progressDialog = ProgressDialog.show(Controlling.this, "Hold on", "Connecting");// http://stackoverflow.com/a/11130220/1287554
        }

        @Override
        protected Void doInBackground(Void... devices) {
            try {
                if (mBTSocket == null || !mIsBluetoothConnected) {
                    if (ActivityCompat.checkSelfPermission(Controlling.this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                        checkPermissions();
                        return null;
                    }
                    mBTSocket = mDevice.createInsecureRfcommSocketToServiceRecord(mDeviceUUID);
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    mBTSocket.connect();
                }
            } catch (IOException e) {
                // Unable to connect to device`
                // e.printStackTrace();
                mConnectSuccessful = false;
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);

            if (!mConnectSuccessful) {
                Toast.makeText(getApplicationContext(), "Could not connect to device.Please turn on your Hardware", Toast.LENGTH_LONG).show();
                finish();
            } else {
                Toast.makeText(getApplicationContext(), "Connected to device", Toast.LENGTH_SHORT).show();
                mIsBluetoothConnected = true;
                //mReadThread = new ReadInput(); // Kick off input reader
            }
            progressDialog.dismiss();
        }
    }

}