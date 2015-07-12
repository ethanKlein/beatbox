/*
  MP3 Shield Trigger
  by: Jim Lindblom
      SparkFun Electronics
  date: September 23, 2013

  This is an example MP3 trigger sketch for the SparkFun MP3 Shield.
  Pins 0, 1, 5, 10, A0, A1, A2, A3, and A4 are setup to trigger tracks
  "track001.mp3", "track002.mp3", etc. on an SD card loaded into
  the shield. Whenever any of those pins are shorted to ground,
  their respective track will start playing.

  When a new pin is triggered, any track currently playing will
  stop, and the new one will start.

  A5 is setup to globally STOP playing a track when triggered.

  If you need more triggers, the shield's jumpers on pins 3 and 4 
  (MIDI-IN and GPIO1) can be cut open and used as additional
  trigger pins. Also, because pins 0 and 1 are used as triggers
  Serial is not available for debugging. Disable those as
  triggers if you want to use serial.

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

/* Pin setup */
/* int triggerPins[TRIGGER_COUNT] = {0, 1, 5, 10, A0, A1, A2, A3, A4}; */
// int triggerPins[TRIGGER_COUNT] = {5, 10, A0, A1, A2, A3, A4};

int toggleSwitch = 10;

int button1 = A5;
int button1led = A4;
int button2 = A3;
int button2led = A2;
int button3 = A1;
int button3led = A0;

int toggleState = 0; 

void setup() {
  Serial.begin(9600);
  pinMode(button1led, OUTPUT);
  pinMode(button2led, OUTPUT);
  pinMode(button3led, OUTPUT);

  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}

void loop() {

  // Serial.println(digitalRead(toggleSwitch));

  if (digitalRead(toggleSwitch) == LOW) {
    toggleState = 0;
  } else {
    toggleState = 1;
  }

  if ((digitalRead(button1) == LOW)) {
    Serial.println("button1 is LOW");    
    stopTrack();
    digitalWrite(button1led, HIGH);
    if (toggleState == 0) {
      MP3player.playMP3("tipsy.mp3");
    } else {
      MP3player.playMP3("boom.mp3");
    }
  } else {
    digitalWrite(button1led, LOW);
  }

  if ((digitalRead(button2) == LOW)) {
    Serial.println("button2 is LOW");
    digitalWrite(button2led, HIGH); // turn on LED
    stopTrack();
    if (toggleState == 0) {
      MP3player.playMP3("tipsy.mp3");
    } else {
      MP3player.playMP3("boom.mp3");
    }
  } else {
    digitalWrite(button2led, LOW); // turn off LED
  }

  if ((digitalRead(button3) == LOW)) {
    Serial.println("button3 is LOW");
    digitalWrite(button3led, HIGH); // turn on LED
    stopTrack();
    if (toggleState == 0) {
      MP3player.playMP3("tipsy.mp3");
    } else {
      MP3player.playMP3("boom.mp3");
    }
  } else {
    digitalWrite(button3led, LOW); // turn off LED
  }

}

// stop any tracks from playing
void stopTrack() {
  if (MP3player.isPlaying()) {
    MP3player.stopTrack();
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
