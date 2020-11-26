
#include "Arduino.h"
#include "ArduinoUnitTests.h"
#include "Keypad_CI.h"

const byte ROWS = 4; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                         {'4', '5', '6', 'B'},
                         {'7', '8', '9', 'C'},
                         {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {5, 4, 3, 2}; // connect to the keypad row pinouts
byte colPins[COLS] = {9, 8, 7, 6}; // connect to the keypad column pinouts

Keypad *keypad;

unittest_setup() {
  keypad = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
}

unittest_teardown() { delete keypad; }

unittest(className) { assertEqual("Keypad_CI", keypad->className()); }

unittest(getKey) {
  assertEqual((int)-1, (int)keypad->getKey());
  keypad->push_back('A');
  assertEqual('A', keypad->getKey());
  assertEqual((int)-1, (int)keypad->getKey());
}

unittest(getKeys) {
  assertFalse(keypad->getKeys());
  keypad->push_back('A');
  assertTrue(keypad->getKeys());
  assertEqual('A', keypad->getKey());
  assertFalse(keypad->getKeys());
}

unittest(getState) { assertEqual(IDLE, keypad->getState()); }

unittest(isPressed) {
  assertFalse(keypad->isPressed('A'));
  keypad->push_back('A');
  assertTrue(keypad->isPressed('A'));
  assertEqual('A', keypad->getKey());
  assertFalse(keypad->isPressed('A'));
}

unittest(waitForKey) {
  keypad->push_back('A');
  assertEqual('A', keypad->waitForKey());
  // keypad->waitForKey();    // assertion failure
}

unittest(keyStateChanged) {
  assertFalse(keypad->keyStateChanged());
  keypad->push_back('A');
  assertTrue(keypad->keyStateChanged());
  assertEqual('A', keypad->getKey());
  assertFalse(keypad->keyStateChanged());
}

unittest(numKeys) {
  assertEqual(0, keypad->numKeys());
  keypad->push_back('A');
  assertEqual(1, keypad->numKeys());
  assertEqual('A', keypad->getKey());
  assertEqual(0, keypad->numKeys());
}

char eventKey = '\0';
int callCount = 0;
void eventListener(char key) {
  eventKey = key;
  ++callCount;
}
unittest(addEventListener) {
  assertEqual((int) 0, (int) eventKey);
  assertEqual(0, callCount);
  keypad->push_back('A');
  assertEqual(1, keypad->numKeys());
  assertEqual('A', keypad->getKey());
  assertEqual(0, keypad->numKeys());
  assertEqual((int) 0, (int) eventKey);
  assertEqual(0, callCount);
  keypad->addEventListener(eventListener);
  assertEqual(0, callCount);
  keypad->push_back('B');
  assertEqual(1, callCount);
  assertEqual('B', eventKey);
  assertEqual('B', keypad->getKey());
  keypad->addEventListener(nullptr);
  keypad->push_back('C');
  assertEqual(1, callCount);
  assertEqual('B', eventKey);
  assertEqual('C', keypad->getKey());
}

unittest_main()
