#include "Keypad_CI.h"
#ifdef MOCK_PINS_COUNT
#include <cassert>

char Keypad_CI::getKey() {
  if (buffer.empty()) {
    return NO_KEY;
  }
  char result = buffer.front();
  buffer.pop_front();
  return result;
}

char Keypad_CI::waitForKey() {
  assert(!buffer.empty());
  char result = buffer.front();
  buffer.pop_front();
  return result;
}

void Keypad_CI::push_back(char keyChar) {
  buffer.push_back(keyChar);
  if (listener) {
    listener(keyChar);
  }
}

#endif
