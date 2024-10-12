/*
When the knock sequence is detected toggle the door from open to closed, triggering the correct relay to 
run the actuator for the appropriate amount of time.

An optional switch can be attached to pin 1, and when this switch is closed, the door actuator is never engaged. RECOMMENDED FOR SAFETY - RUN THIS BACK TO MONITORING ROOM SO YOU CAN EMERGENGY STOP IF CLOSING!

MOM ON switches should be attached to pins 3 and 4 so that they are depressed when the door is fully open or fully closed.
The door will only be considered closed when the close contact switch on pin 3 is depressed. Otherwise the door is considered open.

*/

#include "buttonstate.h"
#include "looptimer.h"
#include "KnockListener.h"

//********************  Constants  ************************************
// Constants for gpio pins
const int _buttonPin       = 2;   // the pin that the pushbutton is attached to
const int _ledPin          = 13;  // on board LED
const int _knockSensorPin  = 0;   // Piezo sensor on pin 0.
const int _maglockRelay    = 3;   // Relay used to control the maglock
const int _overridePin     = 9;   // Override pin for web access


// Use these constants to tune the knock sensor.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int _threshold          = 170;  // Minimum signal from the piezo to register as a knock
const int _rejectValue        = 30;   // If an individual knock is off by this percentage of a knock we don't unlock..
const int _averageRejectValue = 30;   // If the average timing of the knocks is off by this percent we don't unlock.
const int _knockFadeTime      = 150;  // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)

const int _knockComplete = 1200;      // Longest time to wait for a knock before we assume that it's finished.

const int _stateWaiting = 0;
const int _stateListening = 3;
int _operatingState = _stateWaiting;

bool _relayOn = false;
int overrideButtonState;
// debouncers and state objects for the pushbutton as well as the open/close contact buttons
// the numeric argument is the debounce interval in ms
ButtonState _buttonState(_buttonPin, 8);

LoopTimer _timer;

KnockListener _listener(_knockSensorPin, _threshold, _rejectValue, _averageRejectValue, _knockFadeTime, _knockComplete);

//***********************************************************************
//**************************  Setup  ************************************
void setup() {
  // initialize IO pins
  pinMode(_buttonPin, INPUT_PULLUP);
  pinMode(_ledPin, OUTPUT);
  pinMode(_maglockRelay, OUTPUT);
  pinMode(_overridePin, INPUT);

  //set the initial pin values for outputs (relays)
  digitalWrite(_ledPin, LOW);

  Serial.begin(9600);                     // Uncomment the Serial.bla lines for debugging.
  Serial.println("Program start.");       // but feel free to comment them out after it's working right.
  setRelay(false);
  _timer.start();
}

//***********************************************************************
//*************************  MainLoop  **********************************
void loop() {
  // track the time since the last loop iteration
  _timer.tick();
  // main execution loop
  _buttonState.checkButton();
  overrideButtonState = digitalRead(_overridePin);

  // was the pushbutten pressed?
  if (_buttonState.wasDepressed()) {
    Serial.println("Button Pressed.");
    setRelay(true);
  } else if(overrideButtonState == HIGH) {
    Serial.println("Override Activated.");
    setRelay(false);
  }

  // are we listening for followup knocks?
  if (_operatingState == _stateListening) {
    if (_listener.listenTick(_timer.delta())) {
      // we've just finished listening for knocks
      if (_listener.validateKnock()) {
        //WHEN THE KNOCK IS COMPLETE
        Serial.println("VALID KNOCK.");
        setRelay(false);
      } else {
        // return to the neutral waiting state
        _operatingState = _stateWaiting;
        Serial.println("Secret knock failed.");
      }
    }
  }

  // are we waiting for our first knock?
  if (_operatingState == _stateWaiting) {
    if (_listener.listenStart()) {
      Serial.println("Detected an initial knock.");
      // we've detected a knock
      _operatingState = _stateListening;
    }
  }
      
  _buttonState.resetPrevState();
}

void setRelay(bool state){
  _relayOn = state;
  if(_relayOn){
    digitalWrite(_maglockRelay, HIGH);  
  } else {
    digitalWrite(_maglockRelay, LOW);
  }
}
