// Project by Xiao, Cheer, and  MK Skitka
// October 5th, 2021, ITP NYU
// DBR Dance Butt Revolution - Digital Whoopie Cushion + more

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

int sounds[] = {2,3};

// Pressure Sensor Variables
const int analogInPin0 = 0;
const int analogInPin1 = 1;
int sensorValue0 = 0;
int sensorValue1 = 0;

// States & State Variables
const int NOT_SITTING = 0;                      // First Step in the sequence (USER STANDING)
const int SITTING = 1;                          // Second Step                (USER SITTING)
int initial_state = NOT_SITTING;
int current_state = initial_state;
int currentSoundIndex = 0;
int lastSidePressed = -1;                        // -1 means we have not yet pressed a side during this session

                                                // --- Used for calibration of the sensor readings ---

                                                // TODO: Automate 
const int NO_WEIGHT_0 = 350;                    // initial reading of pressure sensor 0 with no weight
const int NO_WEIGHT_1 = 255;                    // initial reading of pressure sensor 1 with no weight

const int LARGE_MOD = 400;
const int MED_MOD = 300;
const int SM_MOD = 50;


void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);                                              // Needs to be at 9600, otherwise we can't write to serial monitor

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {                       // Use softwareSerial to communicate with mp3.
    // Software Serial adds pin 10 & 11 as Serial
    // Communication Pins (communication w/ another device)
    Serial.println(F("ERROR: Unable to begin serial communicationto mp3"));
    while (true);
  }

  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(25);                                          //Set volume value. From 0 to 30
  myDFPlayer.disableLoopAll();
  digitalWrite(analogInPin0, HIGH);                               // write sound data to Speaker

}

void loop()
{
  sensorValue0 = analogRead(analogInPin0);                        // see labels on wires to identify the pressure sensor as 1 or 0 (0-left)
  sensorValue1 = analogRead(analogInPin1);                        // see labels on wires to identify the pressure sensor as 1 or 0 (1-right)
  //myDFPlayer.disableLoop();
  
  // print pressure sensor analog read values (between 0-1023)
  Serial.println("sensorValue 0: " + String(sensorValue0) + "\t" + "sensorValue 1: " + String(sensorValue1) + "\tState: " + current_state);

  // --- When the user first sits down, play opening tune --- //

  if (sensorValue0 > NO_WEIGHT_0 + LARGE_MOD &&
      sensorValue1 > NO_WEIGHT_1 + LARGE_MOD &&
      current_state == NOT_SITTING) {

    lastSidePressed = -1;
    myDFPlayer.playMp3Folder(1);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
    Serial.println("MP3 State: \n\n" + myDFPlayer.readState());

    delay(4000);
    current_state = SITTING;                                       // move on to next step
  }
  // ---  When the user leans to the right, play next tune. Do not repeat.  --- //

  if (sensorValue0 > (NO_WEIGHT_0 + LARGE_MOD) &&
      sensorValue1 < (NO_WEIGHT_1 + MED_MOD) &&
      current_state == SITTING &&
      lastSidePressed != 0) {

    myDFPlayer.playMp3Folder(sounds[currentSoundIndex]);
    lastSidePressed = 0;
    currentSoundIndex = (currentSoundIndex+1) % sizeof(sounds);
    Serial.println("MP3 State: \n\n" + myDFPlayer.readState());
    
    delay(2000); // TODO Need to replace delay value with length of mp3 track
  }
  // --- When the user leans to the left, play next tune. Do not repeat. --- //

  if (sensorValue0 < (NO_WEIGHT_1 + MED_MOD) &&
      sensorValue1 > (NO_WEIGHT_1 + LARGE_MOD) &&
      current_state == SITTING &&
      lastSidePressed != 1) {

    myDFPlayer.playMp3Folder(sounds[currentSoundIndex]);
    lastSidePressed = 1;
    currentSoundIndex = (currentSoundIndex+1) % sizeof(sounds);
    delay(2000);
  }

  // --- When the user stands up play outro tune --- //

  if (sensorValue0 < (NO_WEIGHT_0 + SM_MOD) &&
      sensorValue1 < (NO_WEIGHT_1 + SM_MOD) &&
      current_state == SITTING) {
        
    myDFPlayer.playMp3Folder(4);
    lastSidePressed = -1;
    delay(4000);
    current_state = NOT_SITTING;                                 //move on to next step
  }

}
