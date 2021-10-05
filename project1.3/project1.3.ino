#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Pressure Sensor Values
const int analogInPin0 = 0;
const int analogInPin1 = 1;
int sensorValue0 = 0;
int sensorValue1 = 0;

// States 
const int NOT_SITTING = 0;                 // First Step in the sequence
const int SITTING = 1;                    //Second Step 

int initial_state = NOT_SITTING; 
int current_state = initial_state;
int lastFart = 0;


void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);                                              // Needs to be at 9600, otherwise we can't write to serial monitor

  Serial.println();
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {                       //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin!"));
    while (true);
  }

  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30

  digitalWrite(analogInPin0, HIGH);                               // what does this line do? Do we need it?

}

void loop()
{
  sensorValue0 = analogRead(analogInPin0);                        // see labels on wires to identify the pressure sensor as 1 or 0 (0-left)
  sensorValue1 = analogRead(analogInPin1);                        // see labels on wires to identify the pressure sensor as 1 or 0 (1-right)
  
  Serial.println("sensorValue0: " + String(sensorValue0) + "\t" + "sensorValue 1: " + String(sensorValue1) + "\tState: " + current_state);



  if (sensorValue0 > 700 && sensorValue1 > 700 && current_state == NOT_SITTING) {
    myDFPlayer.playMp3Folder(1);                                   //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
    delay(6000);
    // move on to next step
    current_state = SITTING;
  }
  
  if (sensorValue0 > 700 && sensorValue1 < 620 && current_state == SITTING && lastFart != 2) {
    myDFPlayer.playMp3Folder(2);
    lastFart = 2;                                   
    delay(3000);
  }
  if (sensorValue0 < 400 && sensorValue1 > 700 && current_state == SITTING && lastFart != 3) {
    myDFPlayer.playMp3Folder(lastFart++ % 10); 
    lastFart = 3;                                       
    delay(3000);
  }
  
  if (sensorValue0 < 420 && sensorValue1 < 560 && current_state == SITTING) {
    myDFPlayer.playMp3Folder(4);                                   
    delay(4000);
    //move on to next step
    current_state = NOT_SITTING;
    
  }

}
