# Remote-Controlled-Car
 
#### Authors
- Mercygold Msaki
- Ruth Desta
- Nasim Nakbari
- Aneet Kaur

### Project Image
<p align="center">
<img src = "https://github.com/Mercygoldm/Remote-Controlled-Car/blob/main/Media/Images/Remote%20Controlled%20Car.jpg" width="400" height = "300"/>
</p>

### Hardware & Software Used 
- BeagleBone Green
- Android Phone
- Arduino Uno Board
- BLUETOOTH 2.0 SLAVE MODULE HC-06
- L298N Dual H-Bridge Motor Controller
- Tank Chassis
- Altium Designer

### System Description 
 Our group worked on a remote-controlled car that would move forward, backwards, left, and right in accordance with instructions sent by an Android application. We also implemented functions that allowed us to speed the car up, slow it down, and have it stop moving completely.
 
 Android App -> Bluetooth Module -> Arduino Board -> BBG -> Motor Driver -> Car 
 
 As shown above, we first have our Android application send certain numbers to our Arduino board via a Bluetooth module. The numbers were chosen by us to make the LEDS on our Arduino Board to light up in a certain pattern. The patterns corresponded to a set of directions that our BeagleBoneGreen read through the AIN pins that were connected through a wire from the Arduino circuit. After reading in the voltage values, our code checked them against the directions we had implemented in our DriverModule. We sped up and slowed down the motor using 4 PWM pins on our BeagleBone. The table below shows the patterns of the LEDS we used and what instruction they corresponded to. 
 
 In our project proposal, we had ideas for implementing a video feed that would transmit video from our car to our app, but unfortunately due to lack of time were not able to implement that. We initially had trouble with the original chassis we were using and had to switch to the tank we demoed with a week before the demo because of technical difficulties. We are also not able to speed up when our car turns because we were unable to increase the duty cycle of our turns from a 50% to a 30% split due to issues with finding a powerful enough power source for our car. We had also previously encountered issues with speeding up our Bluetooth data transmission but that was very quickly fixed with (surprisingly!) adding a “\n” to the message we were transmitting. 

[A Video of the Moving Car](https://github.com/Mercygoldm/Remote-Controlled-Car/blob/main/Media/Videos/Moving%20Car.mp4)

[A Screen Record of the Android Phone Controlling the Car](https://github.com/Mercygoldm/Remote-Controlled-Car/blob/main/Media/Videos/Android%20Phone%20Screen%20Recording.mp4)
