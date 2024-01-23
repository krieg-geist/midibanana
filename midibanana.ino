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
#define SCALE_BUTTON 15

const uint8_t LOW_C = 48;
bool octave = false;

// Define scale types
const uint8_t MAJOR_SCALE[] = {0, 2, 4, 5, 7, 9, 11, 12};
const uint8_t MINOR_SCALE[] = {0, 2, 3, 5, 7, 8, 10, 12};
const uint8_t CHROMATIC_SCALE[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};


// Enum to select scale type
// An Enum maps variable names to ascending numbers, 0, 1, 2, etc.
enum ScaleType {
  CHROMATIC,
  MAJOR,
  MINOR,
  NUM_SCALES // This is used to know the number of scale types
};

// Current scale type
ScaleType currentScale = MAJOR;

// Last time the switch was toggled
unsigned long lastSwitchTime = 0;
const unsigned long debounceDelay = 50; // 50 milliseconds debounce delay

// Function to get the note offset based on scale type and position
uint8_t getNoteOffset(uint8_t position) {
    switch(currentScale) {
        case MAJOR:
            return MAJOR_SCALE[position % (sizeof(MAJOR_SCALE) / sizeof(MAJOR_SCALE[0]))];
        case MINOR:
            return MINOR_SCALE[position % (sizeof(MINOR_SCALE) / sizeof(MINOR_SCALE[0]))];
        case CHROMATIC:
            return CHROMATIC_SCALE[position % (sizeof(CHROMATIC_SCALE) / sizeof(CHROMATIC_SCALE[0]))];
        default:
            return 0; // Default to 0 if no scale matches
    }
}
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

  // Set a name for the USB device
  TinyUSBDevice.setProductDescriptor("MIDI BANANA");

  // Configure I2C pins
  Wire.setSDA(16);
  Wire.setSCL(17);

  // Set octave switch pin as input with pull-up
  pinMode(OCTAVE_SWITCH, INPUT_PULLUP);
    
  // Initialize MPR121 with default address
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
  MIDI.sendNoteOn(LOW_C + getNoteOffset(banana) + (octave * 12), 127, 1);
}

void handleNoteOff(uint16_t banana) {
  // Send MIDI note off message
  MIDI.sendNoteOff(LOW_C + getNoteOffset(banana) + (octave * 12), 127, 1);
}

void loop() {
  // Check for octave switch changes
  if (digitalRead(OCTAVE_SWITCH) != octave) {
    octave = !octave;
    MIDI.sendControlChange(123, 0, 1); // Kill all the notes currently playing so we don't accidentally leave them hanging..
  }

  // Check for scale switch changes
  unsigned long currentTime = millis(); // get the time right now
  if (digitalRead(SCALE_BUTTON) == HIGH && currentTime - lastSwitchTime > debounceDelay) {
      lastSwitchTime = currentTime; // Update the last switch time

      // Cycle through the scale types
      currentScale = static_cast<ScaleType>((currentScale + 1) % NUM_SCALES);
      
      // Send MIDI control change to kill all notes
      MIDI.sendControlChange(123, 0, 1);
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
