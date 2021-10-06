#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Pressure Sensor Variables
const int analogInPin0 = 0;
const int analogInPin1 = 1;
int sensorValue0 = 0;
int sensorValue1 = 0;

// States 
const int NOT_SITTING = 0;                // First Step in the sequence (USER STANDING)
const int SITTING = 1;                    // Second Step                (USER SITTING)

int initial_state = NOT_SITTING; 
int current_state = initial_state;
int lastFart = 0;
                                          // --- Used for calibration of the sensor readings --- 
                                          
                                          // TODO: Automate this process
int NO_WEIGHT_0 = 350;                    // initial reading of pressure sensor 0 with no weight 
int NO_WEIGHT_1 = 255;                    // initial reading of pressure sensor 1 with no weight 


void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);                                              // Needs to be at 9600, otherwise we can't write to serial monitor

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {                       // Use softwareSerial to communicate with mp3.
                                                                   // Software Serial adds pin 10 & 11 as Serial Communication Pins (communication w/ another device) 
    Serial.println(F("ERROR: Unable to begin serial communicationto mp3"));                         
    while (true);
  }

  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(25);                                          //Set volume value. From 0 to 30

  digitalWrite(analogInPin0, HIGH);                               // what does this line do? Do we need it?

}

void loop()
{
  sensorValue0 = analogRead(analogInPin0);                        // see labels on wires to identify the pressure sensor as 1 or 0 (0-left)
  sensorValue1 = analogRead(analogInPin1);                        // see labels on wires to identify the pressure sensor as 1 or 0 (1-right)
  
  Serial.println("sensorValue 0: " + String(sensorValue0) + "\t" + "sensorValue 1: " + String(sensorValue1) + "\tState: " + current_state);


  // When the user first sits down, play opening tune   
  if (sensorValue0 > NO_WEIGHT_0 + 400  && sensorValue1 > NO_WEIGHT_1 + 400 && current_state == NOT_SITTING) {
    myDFPlayer.playMp3Folder(1);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
    lastFart = 0;   
    delay(4000);
    // move on to next step
    current_state = SITTING;
  }
   // When the user leans to the right? play 1st fart tune. Do not repeat. 
  if (sensorValue0 > (NO_WEIGHT_0 + 400) && sensorValue1 < (NO_WEIGHT_1 + 300) && current_state == SITTING && lastFart != 2) {
    myDFPlayer.playMp3Folder(2);
    lastFart = 2;                                   
    delay(2000);
  }
   // When the user leans to the left? play 2nd fart tune. Do not repeat. 
  if (sensorValue0 < (NO_WEIGHT_0 + 300) && sensorValue1 > (NO_WEIGHT_1 + 400) && current_state == SITTING && lastFart != 3) {
    myDFPlayer.playMp3Folder(3); 
    lastFart = 3;                                       
    delay(2000);
  }
  // When the user stands up play outro tune 
  if (sensorValue0 < (NO_WEIGHT_0 + 50) && sensorValue1 < (NO_WEIGHT_1 + 50) && current_state == SITTING) {
    myDFPlayer.playMp3Folder(4); 
    lastFart = 0;                                  
    delay(4000);
    //move on to next step
    current_state = NOT_SITTING;
    
  }

}
