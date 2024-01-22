# MIDIBANANA

RING RING RING RING RING RING RING BANANAPHONE

## Overview

This project transforms everyday fruits into a playful and interactive MIDI instrument using the MPR121 capacitive touch sensor. By attaching 12 different fruits (like bananas) to the sensor, each fruit becomes a musical note, allowing you to play melodies just like on a piano but with a fun twist.

## Requirements

### Hardware

- Arduino-compatible microcontroller (RP2040 tested)
- MPR121 Capacitive Touch Sensor
- 12 pieces of conductive material (fruits like bananas, apples, etc.)
- Jumper wires
- USB cable for USB MIDI

### Software

- Arduino IDE
- Adafruit MPR121 Library
- Adafruit TinyUSB Library
- MIDI Library

## Setup

### Hardware Connections

1. Connect the MPR121 sensor to your microcontroller using I2C (SDA to 16, SCL to 17).
2. Connect the ground and VCC of the MPR121 to the microcontroller.
3. Attach each fruit to one of the 12 capacitive touch inputs on the MPR121.
4. Connect the center pin of the octave switch to 14 and the center pin of the scale switch to 15
5. Connect one of the pins of each switch to ground

### Software Configuration

1. Install the required libraries (`Adafruit_MPR121`, `Adafruit_TinyUSB`, `MIDI`) in the Arduino IDE.
2. Load the provided code onto your microcontroller. 
3. If using a MIDI to USB interface, connect the microcontroller to the MIDI device. Be sure to select `Adafruit TinyUSB` under Tools > USB stack
4. If you can't get it to upload, unplug the usb, then plug it back in while holding down the boot button (may be a different button depending on the model of RP2040)

## Usage

- Touching each fruit will send a MIDI note corresponding to its position in the selected musical scale (Major, Minor, or Chromatic).
- The octave can be changed using a designated switch attached to the microcontroller.
- The scale can be changed using a designated switch attached to the microcontroller.

## Code Structure

- `setup()`: Initializes the MPR121 sensor, MIDI configurations, and other necessary setups.
- `loop()`: Constantly checks for touch inputs and sends MIDI signals based on the touched fruit.
- `handleNoteOn()` and `handleNoteOff()`: Handle the sending of MIDI note on and off messages.
- `getNoteOffset()`: Determines the note to be played based on the selected scale and the position of the touch input.

## Customization

- You can modify the `currentScale` variable to switch between different scales.
- Add additional scales by defining them as arrays and adding corresponding cases in the `getNoteOffset` function.

## Contributing

Contributions to this project are welcome. Feel free to fork the repository, make your changes, and submit a pull request.
