// Audiopulse to midiclock. 
// Albert Sikkema, juli 2020
// Free for personal use. Not for commercial use.

// Takes an audio-input from a DAW (f.i. Ableton Live) and converts it into MidiClock, Start and Stop. 
// Resulting Clock is extremeley tight. Much better than Ableton's own midiclock out.
// Added to that a MIDI-in, so you can combine the created midiclock with other midimessages. 
// Everything was loaded unto an Arduino Nano with an ATMEGA328P chip. Midi-in via RX in, MIDI-out via TX-out. Audioclock in via Analog Pin 0.
// Expects short audio pulses, no longer than a few milliseconds. Calculations are based upon 96 clock pulses per 4/4 bar.
// Or 24 clock pulses per quarter note.
// To find out where the pulse is, two thresholds are used: one high threshold (for the peak of the pulse), and one low threshold (for the valley of the pulse). 
// Values between these thresholds are ignored. Both these thresholds can be tweaked based on your audio interface and volume. 
// It's crucial to set these as optimal as possible to prevent mistriggers or insensitivity.

#include <MIDI.h>

// declare constant variables:
const int pulseSensor = A0; // the audio input is connected to analog pin 0
const int highthreshold = 320; 
const int lowthreshold = 150;

// declare variables:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int lastpulseState = LOW;   // stores last state of audio-pulse
int runState = 0; // 0 = STOP / 1 = Running
int lastPulseTime = 0; // time of last pulse
int beatCounter = 1; // not used in this code. Code can be slightly simplified. But is useful for other applications.
int elapsedTime = 0; // Time since last pulse.
int runTimer = 0; 

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
    MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all channels
    MIDI.turnThruOn(); // allow incoming MIDI messages to pass through to MIDI-output
}

void loop() 
{
  MIDI.read(); // Listen to all incoming midi-messages.
  sensorReading = analogRead(pulseSensor);  // read the sensor and store it in the variable sensorReading:
  HandleAnalogRead(sensorReading); // pass sensor value into HandleAnalogRead
}

void HandleAnalogRead(int sensorReading){
  if (sensorReading > highthreshold) { // if pulse is going up above threshold...
    if (lastpulseState == LOW) { 
      if (beatCounter == 1) { 
        if (runState == 0) { // send Start if first audiopulse.
          MIDI.sendStart(); //Start
          MIDI.sendClock(); //Clock
          runState = 1;
        }
        MIDI.sendClock(); //Clock
        beatCounter += 1;
        lastpulseState = HIGH; // Incoming audiopulse is above highest threshold --> lastpulseState = HIGH
        
        
      }
      else if (beatCounter > 1) { // if not first beat
        MIDI.sendClock(); 
        beatCounter += 1;
        lastpulseState = HIGH; 
        runState = 1;
      }

    }
  } 

  else if (sensorReading < lowthreshold) { // Incoming audiopulse is below lowest threshold
    if (lastpulseState == HIGH) {
      lastpulseState = LOW;
      runTimer = 1;
      lastPulseTime = millis();

    }
    else {
      elapsedTime = millis() - lastPulseTime;

      if (runTimer && (elapsedTime > 83)){
        MIDI.sendStop(); //Stop
        beatCounter = 1;
        lastpulseState = LOW;
        runTimer = 0;
        runState = 0;
      }
    }
   }
  // if no clock is present for longer than the time between two pulses, send stop message.
  // max time is dependent on minimum tempo: f.e. 30 bpm => 1 beat is 2 sec, 2000ms => 4 beats = 8 sec. 8/96 = 0,083 => 83 ms 


}
