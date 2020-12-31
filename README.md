# Arduino audio pulse to midi-clock

Convert audiopulse to midiclock with an Arduino Nano

Takes an audio-output from a DAW (f.i. Ableton Live) and converts it into MidiClock, Start and Stop. 
Resulting Clock is extremeley tight. Much better than Ableton's own midiclock out.

Added to that a MIDI-in, so you can combine the created midiclock with other midimessages. 

Everything was loaded unto an Arduino Nano with an ATMEGA328P chip. Midi-in via RX in, MIDI-out via TX-out. Audioclock in via Analog Pin 0.

Expects short audio pulses, no longer than a few milliseconds. Calculations are based upon 96 clock pulses per 4/4 bar.
Or 24 clock pulses per quarter note.

To find out where the pulse is, two thresholds are used: one high threshold (for the peak of the pulse), and one low threshold (for the valley of the pulse). 
Values between these thresholds are ignored. Both these thresholds can be tweaked based on your audio interface and volume. 
It's crucial to set these as optimal as possible to prevent mistriggers or insensitivity!
