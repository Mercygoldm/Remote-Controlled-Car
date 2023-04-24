#include <SoftwareSerial.h>
// Pin2 is RX
// Pin3 is TX
SoftwareSerial HC06BluetoothModule(2, 3);
long int dataReceived;  // stores the number sent from the phone like android app

// PINS
int greenLED_pin = 10; 
int redLED_pin = 12; 
int yellowLED_pin = 13; 

// VALUE RECEIVED
long int Speed_up = 7; 
long int Speed_down = 3; 

long int LEDForward = 8; 
long int LEDBack = 1; 
long int LEDLeft = 4; 
long int LEDRight = 2; 
long int LEDStop = 15; 

void pinMode_initialization() {
  pinMode(greenLED_pin, OUTPUT);
  pinMode(redLED_pin, OUTPUT);
  pinMode(yellowLED_pin, OUTPUT);
}

void allLEDsOff() {
  digitalWrite(greenLED_pin, LOW);
  digitalWrite(redLED_pin, LOW);
  digitalWrite(yellowLED_pin, LOW);
}

void ledStatus_initialization() {
  allLEDsOff();
}

void setup()
{
  pinMode_initialization();
  ledStatus_initialization();

  // Activating serial communication
  Serial.begin(9600);

  // Activating serial communication for bluetooth
  HC06BluetoothModule.begin(9600);
}

void loop()
{
  // Checks if the bluetooth module has not received any information from the android phone the waits
  while (HC06BluetoothModule.available() == 0) ;

  // if the bluetooth module has received some information from the android phone then store the value in the data varaible
  if (HC06BluetoothModule.available() > 0)
  {
    dataReceived = HC06BluetoothModule.parseInt();
  }

  Serial.print(dataReceived);

  if (dataReceived == LEDForward)
  {
    digitalWrite(greenLED_pin, HIGH);
    digitalWrite(redLED_pin, LOW);
    digitalWrite(yellowLED_pin, LOW);
  }
  else if (dataReceived == LEDBack)
  {
    digitalWrite(redLED_pin, LOW);
    digitalWrite(greenLED_pin, LOW);
    digitalWrite(yellowLED_pin, HIGH);
  }
  else if (dataReceived == LEDLeft)
  {

    digitalWrite(redLED_pin, LOW);
    digitalWrite(greenLED_pin, HIGH);
    digitalWrite(yellowLED_pin, HIGH);

  }
  else if (dataReceived == LEDRight)
  {
    digitalWrite(yellowLED_pin, LOW);
    digitalWrite(greenLED_pin, LOW);
    digitalWrite(redLED_pin, HIGH);
  } else if (dataReceived == LEDStop) 
  {
    digitalWrite(yellowLED_pin, HIGH);
    digitalWrite(greenLED_pin, HIGH);
    digitalWrite(redLED_pin, HIGH);
  } 
  else if (dataReceived == Speed_up) 
  {
    digitalWrite(yellowLED_pin, HIGH);
    digitalWrite(greenLED_pin, LOW);
    digitalWrite(redLED_pin, HIGH); 
  } else if (dataReceived == Speed_down) 
  {
    digitalWrite(yellowLED_pin, LOW);
    digitalWrite(greenLED_pin, HIGH);
    digitalWrite(redLED_pin, HIGH);
  }
}
