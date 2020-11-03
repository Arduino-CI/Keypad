/*
bundle config --local path vendor/bundle
bundle install
bundle exec arduino_ci_remote.rb  --skip-compilation
 */
#include <cassert>
#include <vector>
#include "Arduino.h"
#include "ArduinoUnitTests.h"
#include "Keypad.h"
#include "ci/ObservableDataStream.h"

class PinResponder : public DataStreamObserver
{
private:
    byte pinC;
    byte pinR;
    char observer[5] = "pin ";
    bool showData;
    vector<int> pinLog;
    GodmodeState *state;

public:
    // to figure out which button is pressed we need to know which column pin
    // and which row pin are set low. this allows us to choose which pins we want
    // to set low.
    PinResponder(byte pinC, byte pinR)
        : DataStreamObserver(false, false)
    {
        assert(pinC >= 0 && pinC < 10 && pinR >= 0 && pinR < 10);
        observer[4] = '0' + pinC;
        this->pinR = pinR;
        this->pinC = pinC;
        state = GODMODE();
        state->reset();
        state->digitalPin[pinC].addObserver(observer, this);
    }

    ~PinResponder()
    {
        state->digitalPin[pinC].removeObserver(observer);
    }
    // when we get a change on a column, we send the same change on the row to indicate that 
    // the circuit is closed (button is pressed)
    virtual void onBit(bool aBit)
    {
        state->digitalPin[pinR] = aBit;
    }

    virtual String observerName() const { return observer; }
};

// ----------------------------------------------------------------------------
//Create a keyboard based on what is used in open-acidification
//project. Looks like this:
//                            { 1 2 3 A }
//                            { 4 5 6 B }
//                            { 7 8 9 C }
//                            { * 0 # D }
//
// This keyboard requires 8 pins to communicate with the Arduino
// Which buttons are pressed. In the code. below..
// column pin 9 corresponds to keypad buttons { 1, 4, 7, * }
// column pin 8 corresponds to keypad buttons { 2, 5, 8, 0 }
// column pin 7 corresponds to keypad buttons { 3, 6, 9, # }
// column pin 6 corresponds to keypad buttons { A, B, C, D }
// row pin 5 corresponds to keypad buttons { 1, 2, 3, A }
// row pin 4 corresponds to keypad buttons { 4, 5, 6, B }
// row pin 3 corresponds to keypad buttons { 7, 8, 9, C }
// row pin 2 corresponds to keypad buttons { *, 0, #, D }
//
// The specification of the keypad code sets a button to be active when set LOW.
// With this we can check if a button is pressed by looking for a column
// pin set LOW and then looking for the row pin set LOW.
//
// For example if we see pin 9 and 3 LOW then button '7' is common between them.
// This code tests all those pairs. Here are the results
// in the form (column pin, row pin) = button:
// (9,5) = '1', (8,5) = '2', (7,5) = '3', (6,5) = 'A',
// (9,4) = '4', (8,4) = '5', (7,4) = '6', (6,4) = 'B',
// (9,3) = '7', (8,3) = '8', (7,3) = '9', (6,3) = 'C',
// (9,2) = '*', (8,2) = '0', (7,2) = '#', (6,2) = 'D',
// ------------------------------------------------------------------------------

void testKey(byte column, byte row, char key, Keypad keypad)
{
    GodmodeState *state;
    state = GODMODE();
    PinResponder *pin;
    pin = new PinResponder(column, row);     // row pin follows the column pin
    delay(11); // the library defines a debounce delay of 10ms before recognizing any activity
    for (int i = 2; i <= 5; ++i)    // Set all rows to HIGH (not pressed)
    {
        state->digitalPin[i] = HIGH;
    }
    // getKey() will toggle all the columns and see which rows were triggered
    assert((int)key == (int)keypad.getKey());
    // remove active pin and allow the key to cycle through states to IDLE
    delete pin;
    pin = nullptr;
    delay(11);
    keypad.getKey();
    delay(11);
    keypad.getKey();
}

unittest(init)
{

    const byte ROWS = 4; //four rows
    const byte COLS = 4; //three columns
    char keys[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};
    byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
    byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

    Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
    assertFalse(keypad.isPressed('1'));

    testKey(6, 5, 'A', keypad);
    testKey(6, 4, 'B', keypad);
    testKey(6, 3, 'C', keypad);
    testKey(6, 2, 'D', keypad);
    testKey(7, 5, '3', keypad);
    testKey(7, 4, '6', keypad);
    testKey(7, 3, '9', keypad);
    testKey(7, 2, '#', keypad);
    testKey(8, 5, '2', keypad);
    testKey(8, 4, '5', keypad);
    testKey(8, 3, '8', keypad);
    testKey(8, 2, '0', keypad);
    testKey(9, 5, '1', keypad);
    testKey(9, 4, '4', keypad);
    testKey(9, 3, '7', keypad);
    testKey(9, 2, '*', keypad);
}

unittest_main()
