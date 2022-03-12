#define FUNCTIONAL 1
#define NRBUTTONS 8
#define NRLEDS 3
#define NRPOTS 6
#define TIMEOUT 300
#define POTTHRESHHOLD 10
#include <MIDI.h>
#ifdef FUNCTIONAL
  MIDI_CREATE_DEFAULT_INSTANCE();
#endif  
const int buttonPins[NRBUTTONS] = {3, 4, 5, 6, 7, 8, 9, 10};
const int potPins[NRPOTS] = {A5, A4, A3, A2, A1, A0};
const int ledPins[NRLEDS] = {2, 11, 12};
int currentButtonValues[NRBUTTONS] = {};
int previousButtonValues[NRBUTTONS] = {};
int currentPotValues[NRPOTS] = {0};
int previousPotValues[NRPOTS] = {0};
int potDiff = 0;
int currentMidiValues[NRPOTS] = {0};
int previousMidiValues[NRPOTS] = {0};
unsigned long toggleTime[NRBUTTONS] = {0};
unsigned long toggleDelay = 50;
boolean potMoving = true;
unsigned long previousTimer[NRPOTS] = {0};
unsigned long timer[NRPOTS] = {0}; 
bool isPlaying = false;
byte midiCh = 1; //* MIDI channel to be used
byte note = 1; //* Lowest note to be used
byte cc = 1; //* Lowest MIDI CC to be used


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  #ifdef DEBUG
  Serial.println("Debug mode");
  Serial.println();
  #endif
  for (int i = 0; i < NRBUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(2, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  buttonManager();
  potManager();
  ledManager();
}
void buttonManager() {
  for (int i = 0; i < NRBUTTONS; i++) {
    currentButtonValues[i] = digitalRead(buttonPins[i]);

    if ((millis() - toggleTime[i]) > toggleDelay) {
      if (previousButtonValues[i] != currentButtonValues[i]) {
        toggleTime[i] = millis();

        if (currentButtonValues[i] == LOW) {
          if (buttonPins[i] == 7 && isPlaying) {
            isPlaying = false;
          } else if(buttonPins[i] != 7 && !isPlaying) {
            isPlaying = true;
          }
          #ifdef FUNCTIONAL
            MIDI.sendNoteOn(note + i, 127, midiCh);
          #elif DEBUG
            Serial.print(i);
            Serial.println(": button on"); 
          #endif   
        } else {
          #ifdef FUNCTIONAL
            MIDI.sendNoteOn(note + i, 0, midiCh);
          #elif DEBUG
            Serial.print(i);
            Serial.println(": button off");
          #endif  
              
        }
        previousButtonValues[i] = currentButtonValues[i];
      }
    }
  }
}
void potManager() {
  for (int i = 0; i < NRPOTS; i++) {
    currentPotValues[i] = analogRead(potPins[i]);
    currentMidiValues[i] = map(currentPotValues[i], 0, 1023, 0, 127);

    potDiff = abs(currentPotValues[i] - previousPotValues[i]);
    if (potDiff > POTTHRESHHOLD) {
      previousTimer[i] = millis();
    }
    timer[i] = millis() - previousTimer[i];
    if (timer[i] >= TIMEOUT) {
      potMoving = false;
    } else {
      potMoving = true;
    }

    if (potMoving) {
      if (currentMidiValues[i] != previousMidiValues[i]) {
        #ifdef FUNCTIONAL
          MIDI.sendControlChange(cc + i, currentMidiValues[i], midiCh);
        #elif DEBUG
          Serial.print("Pot: ");
          Serial.print(i);
          Serial.print(" ");
          Serial.println(currentMidiValues[i]);
        #endif
        previousPotValues[i] = currentPotValues[i];
        previousMidiValues[i] = currentMidiValues[i];    
      }
    }
  }
}

void ledManager() {
  if (isPlaying) {
    for (int i = 0; i < NRLEDS; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(50);
    }
    for (int i = 0; i < NRLEDS; i++) {
     digitalWrite(ledPins[i], LOW);
     delay(50);
   }
  }
  
}
  

 
