#pragma once
#include <Keypad.h>
#ifdef MOCK_PINS_COUNT
#include <deque>

class Keypad_CI : public Keypad_Base {
public:
  Keypad_CI(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols)
      : Keypad_Base(userKeymap, row, col, numRows, numCols) {
    listener = nullptr;
  }
  char getKey();
  bool getKeys() { return !buffer.empty(); }
  KeyState getState() { return IDLE; }
  bool isPressed(char keyChar) {
    return buffer.empty() ? false : buffer.front() == keyChar;
  }
  void addEventListener(void (*function)(char)) { listener = function; }
  char waitForKey();
  bool keyStateChanged() { return !buffer.empty(); }
  byte numKeys() { return buffer.empty() ? 0 : 1; }

  // testing support
  virtual String className() const { return "Keypad_CI"; }
  void push_back(char keyChar);

private:
  void (*listener)(char);
  std::deque<char> buffer;
};

#endif
