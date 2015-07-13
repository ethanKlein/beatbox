/*
  Beatbox code for the Klein Boys.
  
  Some code borrowed from the:
  MP3 Shield Trigger
  by: Jim Lindblom
      SparkFun Electronics
  date: September 23, 2013

  This is an example MP3 trigger sketch for the SparkFun MP3 Shield.

  When a new pin is triggered, any track currently playing will
  stop, and the new one will start.

  If you need more triggers, the shield's jumpers on pins 3 and 4 
  (MIDI-IN and GPIO1) can be cut open and used as additional
  trigger pins.

  Much of this code was grabbed from the FilePlayer example
  included with the SFEMP3Shield library. Major thanks to Bill
  Porter and Michael Flaga, again, for this amazing library!
*/

#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; // Create object to handle SD functions

SFEMP3Shield MP3player; // Create Mp3 library object
// These variables are used in the MP3 initialization to set up
// some stereo options:
const uint8_t volume = 0; // MP3 Player volume 0=max, 255=lowest (off)
const uint16_t monoMode = 1;  // Mono setting 0=off, 3=max

#define DEBOUNCE 5  // button debouncer

// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {A5, A3, A1};

// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)

// we will track if a button is just pressed, just released, or 'pressed' (the current state
volatile byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];


// ethan variables
int toggleSwitch = 10;
int button1 = A5;
int button2 = A3;
int button3 = A1;


int led1 = A4;
int led2 = A2;
int led3 = A0;
int currButton = A5;
// char* bank1[] = {"XAVI.WAV", "LASER.WAV", "SUSP.WAV"};
// char* bank2[] = {"ISLAND.WAV", "BLUES.WAV", "PARTY.WAV"};
char* bank1[] = {"laser.mp3", "laser.mp3", "laser.mp3"};
char* bank2[] = {"tipsy.mp3", "tipsy.mp3", "tipsy.mp3"};
boolean buttonFirst[] = {true, true, true};


int inPin = 10;         // the number of the input pin (toggle switch)
int toggleState = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

int buttonState = HIGH;      // the current state of the output pin
int buttonReading;           // the current reading from the input pin
int buttonPrevious = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

void setup() {
  Serial.begin(9600);
  byte i;
  
  // LEDs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  // Toggle switch
  pinMode(inPin, INPUT);
 
  // Make input & enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }

  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
  
}

SIGNAL(TIMER2_OVF_vect) {
  //check_switches();
}

void check_switches() {
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  byte index;

  for (index = 0; index < NUMBUTTONS; index++) {
    currentstate[index] = digitalRead(buttons[index]);   // read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW) && millis() - time > debounce) {
          justpressed[index] = 1; // just pressed
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH) && millis() - time > debounce) {
          justreleased[index] = 1; // just released
      }
      pressed[index] = currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}


void ledOn(int led) {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  // delay(100);               // wait for a second
  // delay(1000);  
}

void ledOff(int led) {
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
}

// if no mp3 file playing, change currWave to empty string. 
void checkIfPlaying() {
  if (!MP3player.isPlaying()) {
    currButton=A5;
  }
}

void checkToggle() {
  reading = digitalRead(inPin);
  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember the time
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (toggleState == HIGH)
      toggleState = LOW;
    else
      toggleState = HIGH;
    time = millis();    
  }
 if (reading == LOW && previous == HIGH && millis() - time > debounce) {
   if (toggleState == LOW)
     toggleState = HIGH;
   else
     toggleState = LOW;
   time = millis();    
 } 
  previous = reading;  
}


void loop() {
  byte i;
  check_switches();
  checkIfPlaying(); 
  checkToggle();

  if (pressed[0] == 0) {
    Serial.println("button 0");
    ledOn(led1);
    playfile(0);
    buttonFirst[0] = false;
  } else {
    ledOff(led1);
    buttonFirst[0] = true;
  }

  if (pressed[1] == 0) {
   Serial.println("button 1"); 
   ledOn(led2);
   playfile(1);
   buttonFirst[1] = false;
  } else {
    ledOff(led2);
    buttonFirst[1] = true;
  }

  if (pressed[2] == 0) {
    Serial.println("button 2");
    ledOn(led3);
    playfile(2);
    buttonFirst[2] = false;
  } else {
    ledOff(led3);
    buttonFirst[2] = true;
  }
}

// stop any tracks from playing
void stopTrack() {
  if (MP3player.isPlaying()) {
    MP3player.stopTrack();
  }
}

void playfile(int buttonNum) {
  int testButton = buttonNum;
  if (testButton == currButton) { // same button is pressed
    if (buttonFirst[buttonNum] == true) {
      stopTrack();
      if (toggleState == HIGH) {
        Serial.println("should be playing tipsy");
        MP3player.playMP3("tipsy.mp3");
        // MP3player.playMP3(bank1[buttonNum]);
      } else {
        MP3player.playMP3(bank2[buttonNum]);
      }
    }
  } else {
    currButton = buttonNum;
    stopTrack();
    if (toggleState == HIGH) {
      Serial.println("should be playing tipsy");
      MP3player.playMP3("tipsy.mp3");

      // MP3player.playMP3(bank1[buttonNum]);
    } else {
      MP3player.playMP3(bank2[buttonNum]);
    }
  }
}

// initSD() initializes the SD card and checks for an error.
void initSD() {
  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) 
    sd.initErrorHalt();
  if(!sd.chdir("/")) 
    sd.errorHalt("sd.chdir");
}

// initMP3Player() sets up all of the initialization for the
// MP3 Player Shield. It runs the begin() function, checks
// for errors, applies a patch if found, and sets the volume/
// stero mode.
void initMP3Player() {
  uint8_t result = MP3player.begin(); // init the mp3 player shield
  if(result != 0) // check result, see readme for error codes.
  {
    // Error checking can go here!
  }
  MP3player.setVolume(volume, volume);
  MP3player.setMonoMode(monoMode);
}







