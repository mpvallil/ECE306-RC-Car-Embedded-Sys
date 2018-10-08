//------------------------------------------------------------------------------
//
//  Description: This file contains code for an application to control an RC 
//  controlled by an MSP430 microcontroller
//
//
//
//  Matthew Vallillo
//  April 2018
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
package mpv.vroom;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.Socket;
import java.net.URL;
import java.net.URLConnection;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    //Buttons
    private Button save_IP_button;
    private Button forward_button;
    private Button backward_two_seconds_button;
    private Button stop_button;
    private Button nudge_right_button;
    private Button nudge_left_button;
    private Button follow_line_button;
    private Button exit_line_button;
    private Button left_PWM_button;
    private Button right_PWM_button;

    //EditText
    private EditText save_IP_editText;
    private EditText save_PWM_editText;

    //TextView
    private TextView status_textview;

    //ip Address
    private String ip_address;

    //command variable
    String command;

    //Strings
    String fwd = "Forward";
    String bckw = "Backward";
    String rt = "Right";
    String lft = "Left";
    String fl = "Following Line";
    String ex = "Exiting Line";
    String rpwm = "Right PWM Changed";
    String lpwm = "Left PWM Changed";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Buttons
        save_IP_button = findViewById(R.id.button_save_IP);
        forward_button = findViewById(R.id.forward);
        backward_two_seconds_button = findViewById(R.id.backward_two_seconds);
        stop_button = findViewById(R.id.button_stop);
        nudge_left_button = findViewById(R.id.button_nudge_left);
        nudge_right_button = findViewById(R.id.button_nudge_right);
        follow_line_button = findViewById(R.id.button_follow_line);
        exit_line_button = findViewById(R.id.button_exit_line);
        left_PWM_button = findViewById(R.id.button_pwm_left);
        right_PWM_button = findViewById(R.id.button_pwm_right);


        //EditText
        save_IP_editText = findViewById(R.id.editText_save_IP);
        save_PWM_editText = findViewById(R.id.editText_save_PWM);

        //TextView
        status_textview = findViewById(R.id.status_text);

        //ClickListeners
        save_IP_button.setOnClickListener(save_IP_ButtonListener);
        stop_button.setOnClickListener(stop_button_ButtonListener);
        follow_line_button.setOnClickListener(follow_line_button_ButtonListener);
        exit_line_button.setOnClickListener(exit_line_button_ButtonListener);
        left_PWM_button.setOnClickListener(left_PWM_button_ButtonListener);
        right_PWM_button.setOnClickListener(right_PWM_button_ButtonListener);

        //TouchListeners
        forward_button.setOnTouchListener(forward_TouchListener);
        backward_two_seconds_button.setOnTouchListener(backward_two_seconds_TouchListener);
        nudge_left_button.setOnTouchListener(nudge_left_button_TouchListener);
        nudge_right_button.setOnTouchListener(nudge_right_button_TouchListener);

    }

    // Creates a Listener to save the IP address input from the save_IP_editText textbox
    public View.OnClickListener save_IP_ButtonListener = new View.OnClickListener() {

        public void onClick(View v) {

        ip_address = save_IP_editText.getText().toString();
        String ipaddr = "Saved ip";
        System.out.println(ipaddr);
        }
    };

    // Creates a listener to send the command to move the car forward
    // Will continue to move forward as long as button is held down
    public View.OnTouchListener forward_TouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            command = ".1202F0000";
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                CommandThread ct = new CommandThread(command, ip_address);
                ct.start();
                status_textview.setText(fwd);
            } else if (event.getAction() == MotionEvent.ACTION_UP) {
                CommandThread ct = new CommandThread(".1202S0000", ip_address);
                ct.start();
                status_textview.setText("");
            }
            return true;
        }
    };

    // Creates a listener to send the command to move the car backward
    // Will continue to move backward as long as button is held down
    public View.OnTouchListener backward_two_seconds_TouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            command = ".1202B0000";
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                CommandThread ct = new CommandThread(command, ip_address);
                ct.start();
                status_textview.setText(bckw);
            } else if (event.getAction() == MotionEvent.ACTION_UP) {
                CommandThread ct = new CommandThread(".1202S0000", ip_address);
                ct.start();
                status_textview.setText("");
            }
            return true;
        }
    };

    // Creates a listener to send the command to stop the car
    public View.OnClickListener stop_button_ButtonListener = new View.OnClickListener() {

        public void onClick(View v) {

        String command = ".1202S0000";
        CommandThread ct = new CommandThread(command, ip_address);
        ct.start();
            status_textview.setText("");
        }
    };

    // Sends command to take the pulse width modulation value (max = 1000) of the left wheel
    // Affects speed of left wheel
    public View.OnClickListener left_PWM_button_ButtonListener = new View.OnClickListener() {

        public void onClick(View v) {
            String pwm = save_PWM_editText.getText().toString();
            String command = null;
            if (pwm.length() == 4) {
                command = ".1202C" + pwm;
            }
            CommandThread ct = new CommandThread(command, ip_address);
            ct.start();
            status_textview.setText(lpwm);
        }
    };

    // Sends command to take the pulse width modulation value (max = 1000) of the right wheel
    // Affects speed of right wheel
    public View.OnClickListener right_PWM_button_ButtonListener = new View.OnClickListener() {

        public void onClick(View v) {
            String pwm = save_PWM_editText.getText().toString();
            String command = null;
            if (pwm.length() == 4) {
                command = ".1202D" + pwm;
            }
            CommandThread ct = new CommandThread(command, ip_address);
            ct.start();
            status_textview.setText(rpwm);
        }
    };

    // Sends command to initiate the routine to start following the blackline autonomously
    public View.OnClickListener follow_line_button_ButtonListener = new View.OnClickListener() {

        public void onClick(View v) {

        String command = ".1202A0000";
        CommandThread ct = new CommandThread(command, ip_address);
        ct.start();
        status_textview.setText(fl);
        }
    };

    // Sends command to stop following the black line
    public View.OnClickListener exit_line_button_ButtonListener = new View.OnClickListener() {

        public void onClick(View v) {

        String command = ".1202E0000";
        CommandThread ct = new CommandThread(command, ip_address);
        ct.start();
        status_textview.setText(ex);

        }
    };

}

// Starts a Thread that sends the HTTP message to send all the commands to the car
class CommandThread extends Thread {
    private String command;
    private String ip;

    CommandThread(String command, String ip) {
        this.command = command;
        this.ip = ip;
    }

    public void run() {
        HttpURLConnection command_url = null;
        InputStream is = null;
        try {
            String command_address = "http://" + ip + "/output.cgi?text=" + command + "&submit=Submit";
            command_url = (HttpURLConnection) new URL(command_address).openConnection();
            is = command_url.getInputStream();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (command_url != null) {
                command_url.disconnect();
            }
            if (is != null) {
                try {
                    is.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}


