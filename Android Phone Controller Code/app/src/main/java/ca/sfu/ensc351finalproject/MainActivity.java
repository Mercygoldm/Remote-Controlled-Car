package ca.sfu.ensc351finalproject;

import android.Manifest;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Build;
import android.preference.PreferenceManager;
//import android.support.v7.app.AppCompatActivity;
import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import pub.devrel.easypermissions.EasyPermissions;

/*
 * Line 70 to line 189 is our code
 * After line 189 is a mixture of our code and some code from this link:-
   https://www.electroniclinic.com/android-app-development-to-control-arduino-over-bluetooth-using-android-studio/
 */

public class MainActivity extends AppCompatActivity {
    private Button search;
    private Button connect;
    private ListView listView;
    private BluetoothAdapter mBTAdapter;
    private static final int BT_ENABLE_REQUEST = 10; // This is the code we use for BT Enable
    private static final int SETTINGS = 20;
    private UUID mDeviceUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private int mBufferSize = 50000; //Default
    public static final String DEVICE_EXTRA = "com.example.lightcontrol.SOCKET";
    public static final String DEVICE_UUID = "com.example.lightcontrol.uuid";
    private static final String DEVICE_LIST = "com.example.lightcontrol.devicelist";
    private static final String DEVICE_LIST_SELECTED = "com.example.lightcontrol.devicelistselected";
    public static final String BUFFER_SIZE = "com.example.lightcontrol.buffersize";
    private static final String TAG = "BlueTest5-MainActivity";
    private final int LOCATION_PERMISSION_REQUEST = 101;


    public static final String[] BLUETOOTH_PERMISSIONS_S = {Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setUpToolBar();
        launchWelcomeScreen();
        initBluetooth();
        checkPermissions();

        search = findViewById(R.id.search);
        connect = findViewById(R.id.connect);
        listView = findViewById(R.id.listview);

        findViewById(R.id.resource).setOnClickListener(v-> launchResourceScreen());

        if (savedInstanceState != null) {
            ArrayList<BluetoothDevice> list = savedInstanceState.getParcelableArrayList(DEVICE_LIST);
            if (list != null) {
                initList(list);
                MyAdapter adapter = (MyAdapter) listView.getAdapter();
                int selectedIndex = savedInstanceState.getInt(DEVICE_LIST_SELECTED);
                if (selectedIndex != -1) {
                    adapter.setSelectedIndex(selectedIndex);
                    connect.setEnabled(true);
                }
            } else {
                initList(new ArrayList<BluetoothDevice>());
            }
        } else {
            initList(new ArrayList<BluetoothDevice>());
        }

        search.setOnClickListener(v-> searchForBluetoothDevices());
        connect.setOnClickListener(v-> connectBluetoothDevice());
    }

    private void initBluetooth() {
        mBTAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBTAdapter == null) {
            Toast.makeText(this, "No Bluetooth found", Toast.LENGTH_SHORT).show();
        }
        if (mBTAdapter.isEnabled()) {
            Toast.makeText(this, "Bluetooth already enabled", Toast.LENGTH_SHORT).show();
        } else {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                checkPermissions();
                return;
            }
            mBTAdapter.enable();
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
                                MainActivity.this.finish();
                            }
                        }).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void launchWelcomeScreen() {
        Intent intent1 = WelcomeScreenActivity.makeWelcomeIntent(MainActivity.this);
        startActivity(intent1);
    }
    private void setUpToolBar() {
        setTitle("Connect to the Car");
        ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Connect to the Car");
    }

    private void launchResourceScreen() {
        Intent intent = ResourceActivity.makeResourceActivityIntent(MainActivity.this);
        startActivity(intent);
    }

    private void connectBluetoothDevice() {
        BluetoothDevice device;
        try {
            device = ((MyAdapter) (listView.getAdapter())).getSelectedItem();
            Intent intent = new Intent(getApplicationContext(), Controlling.class);
            intent.putExtra(DEVICE_EXTRA, device);
            intent.putExtra(DEVICE_UUID, mDeviceUUID.toString());
            intent.putExtra(BUFFER_SIZE, mBufferSize);
            startActivity(intent);
        }catch (Exception exception){
            Toast.makeText(this,"No Bluetooth Device Selected",Toast.LENGTH_SHORT).show();
        }
    }

    private void searchForBluetoothDevices() {
        mBTAdapter = BluetoothAdapter.getDefaultAdapter();

        if (mBTAdapter == null) {
            Toast.makeText(getApplicationContext(), "Bluetooth not found", Toast.LENGTH_SHORT).show();
        } else if (!mBTAdapter.isEnabled()) {
            Intent enableBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            if (ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                checkPermissions();
                return;
            }
            startActivityForResult(enableBT, BT_ENABLE_REQUEST);
        } else {
            new SearchDevices().execute();
        }
    }

    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case BT_ENABLE_REQUEST:
                if (resultCode == RESULT_OK) {
                    msg("Bluetooth Enabled successfully");
                    new SearchDevices().execute();
                } else {
                    msg("Bluetooth couldn't be enabled");
                }

                break;
            case SETTINGS: //If the settings have been updated
                SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
                String uuid = prefs.getString("prefUuid", "Null");
                mDeviceUUID = UUID.fromString(uuid);
                Log.d(TAG, "UUID: " + uuid);
                String bufSize = prefs.getString("prefTextBuffer", "Null");
                mBufferSize = Integer.parseInt(bufSize);
                break;
            default:
                break;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    /**
     * Quick way to call the Toast
     *
     * @param str
     */
    private void msg(String str) {
        Toast.makeText(getApplicationContext(), str, Toast.LENGTH_SHORT).show();
    }

    /**
     * Initialize the List adapter
     *
     * @param objects
     */
    private void initList(List<BluetoothDevice> objects) {
        final MyAdapter adapter = new MyAdapter(getApplicationContext(), R.layout.list_item_2, R.id.lstContent2, objects);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                adapter.setSelectedIndex(position);
                connect.setEnabled(true);
            }
        });
    }

    /**
     * Searches for paired devices. Doesn't do a scan! Only devices which are paired through Settings->Bluetooth
     * will show up with this. I didn't see any need to re-build the wheel over here
     *
     * @author ryder
     */
    private class SearchDevices extends AsyncTask<Void, Void, List<BluetoothDevice>> {

        @Override
        protected List<BluetoothDevice> doInBackground(Void... params) {
            if (ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                checkPermissions();
                return null;
            }
            Set<BluetoothDevice> pairedDevices = mBTAdapter.getBondedDevices();
            List<BluetoothDevice> listDevices = new ArrayList<BluetoothDevice>();
            for (BluetoothDevice device : pairedDevices) {
                listDevices.add(device);
            }
            return listDevices;

        }

        @Override
        protected void onPostExecute(List<BluetoothDevice> listDevices) {
            super.onPostExecute(listDevices);
            if (listDevices.size() > 0) {
                MyAdapter adapter = (MyAdapter) listView.getAdapter();
                adapter.replaceItems(listDevices);
            } else {
                msg("No paired devices found, please pair your serial BT device and try again");
            }
        }

    }

    /**
     * Custom adapter to show the current devices in the list. This is a bit of an overkill for this
     * project, but I figured it would be good learning
     * Most of the code is lifted from somewhere but I can't find the link anymore
     *
     * @author ryder
     */
    private class MyAdapter extends ArrayAdapter<BluetoothDevice> {
        private int selectedIndex;
        private Context context;
        private int selectedColor = Color.parseColor("#abcdef");
        private List<BluetoothDevice> myList;

        public MyAdapter(Context ctx, int resource, int textViewResourceId, List<BluetoothDevice> objects) {
            super(ctx, resource, textViewResourceId, objects);
            context = ctx;
            myList = objects;
            selectedIndex = -1;
        }

        public void setSelectedIndex(int position) {
            selectedIndex = position;
            notifyDataSetChanged();
        }

        public BluetoothDevice getSelectedItem() {
            return myList.get(selectedIndex);
        }

        @Override
        public int getCount() {
            return myList.size();
        }

        @Override
        public BluetoothDevice getItem(int position) {
            return myList.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        private class ViewHolder {
            TextView tv;
        }

        public void replaceItems(List<BluetoothDevice> list) {
            myList = list;
            notifyDataSetChanged();
        }

        public List<BluetoothDevice> getEntireList() {
            return myList;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View vi = convertView;
            ViewHolder holder;
            if (convertView == null) {
                vi = LayoutInflater.from(context).inflate(R.layout.list_item_2, null);

               // LinearLayout linearLayout = vi.findViewById(R.id.linearLayout2);
               // linearLayout.setBackground(ContextCompat.getDrawable(MainActivity.this,R.drawable.available_bluetooth_background));
                holder = new ViewHolder();

                holder.tv = (TextView) vi.findViewById(R.id.lstContent2);

                vi.setTag(holder);
            } else {
                holder = (ViewHolder) vi.getTag();
            }

            if (selectedIndex != -1 && position == selectedIndex) {
                vi.findViewById(R.id.lstContent2).setBackgroundColor(R.color.teal_200);
            } else {
                holder.tv.setBackgroundColor(Color.WHITE);
            }
            BluetoothDevice device = myList.get(position);
            if (ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                checkPermissions();
                return null;
            }
            holder.tv.setText(device.getName() + "\n " + device.getAddress());

            return vi;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return true;
    }
}
