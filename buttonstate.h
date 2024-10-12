// Requires the Bounce2 library from https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include <Bounce2.h>

class ButtonState
{
  public:
    ButtonState(int buttonPin, int debounceInterval) {
      _debouncer.attach(buttonPin);
      _debouncer.interval(debounceInterval);
      _debouncer.update();
      _state = _prevState = _debouncer.read();
    }

    // return the logical value of the button pin
    bool value() { return _state; };

    // these two are for our MOM ON switches where false means pressed
    // true if the button is pressed, but its previous state is unpressed
    bool wasDepressed() { return (_state != _prevState) && (_state == false); }
    // true if the button is unpressed, but its previous state is pressed
    bool wasReleased() { return (_state != _prevState) && (_state == true); }

    // read an updated value from the button pin
    bool checkButton() {
      _debouncer.update();
      bool buttonState = _debouncer.read();
      if (buttonState != _state) {
        Serial.println(buttonState);
      }
      _state = buttonState;
      return buttonState;
    }

    // set the previous button state to the current state
    void resetPrevState() { _prevState = _state; }
    
  private:
    Bounce _debouncer = Bounce();
    bool _state;
    bool _prevState;
};

