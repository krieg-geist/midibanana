#include "Adafruit_MPR121.h"
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// Define the USB MIDI object
Adafruit_USBD_MIDI usb_midi(1);

// Create a MIDI instance
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// Macro for bit value
#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// Define constants for octave switching
#define OCTAVE_SWITCH 14
const uint8_t LOW_C = 48;
bool octave = false;

// Define the notes of a major scale
const uint8_t MAJOR_SCALE[] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 18};

// Initialize capacitive touch sensor
Adafruit_MPR121 cap = Adafruit_MPR121();

// Variables to keep track of the last touched and currently touched pins
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() {
  // Initialize TinyUSB on non-supported cores like mbed rp2040
  #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    TinyUSB_Device_Init(0);
  #endif

  // Set product descriptor for the USB device
  TinyUSBDevice.setProductDescriptor("MIDI BANANA");

  // Configure I2C pins
  Wire.setSDA(16);
  Wire.setSCL(17);

  // Set octave switch pin as input with pull-up
  pinMode(OCTAVE_SWITCH, INPUT_PULLUP);
    
  // Initialize MPR121 with default or specified address
  if (!cap.begin(0x5A)) {
    while (1); // Hang if initialization fails
  }

  // Initialize Serial and MIDI communication
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Wait until the USB device is mounted
  while (!TinyUSBDevice.mounted()) delay(1);
}

void handleNoteOn(uint16_t banana) {
  // Send MIDI note on message
  MIDI.sendNoteOn(LOW_C + MAJOR_SCALE[banana] + (octave * 12), 127, 1);
}

void handleNoteOff(uint16_t banana) {
  // Send MIDI note off message
  MIDI.sendNoteOff(LOW_C + MAJOR_SCALE[banana] + (octave * 12), 127, 1);
}

void loop() {
  // Check for octave switch changes
  if (digitalRead(OCTAVE_SWITCH) != octave) {
    octave = !octave;
    MIDI.sendControlChange(123, 0, 1); // Send a MIDI control change
  }

  // Get the currently touched pads
  currtouched = cap.touched();
  
  // Iterate over the 12 possible touch inputs
  for (uint8_t i = 0; i < 12; i++) {
    // Check for touch press and release events
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i))) {
      handleNoteOn(i);
    }
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i))) {
      handleNoteOff(i);
    }
  }

  // Update the last touched state
  lasttouched = currtouched;

  // Short delay for stability
  delay(1);
}
