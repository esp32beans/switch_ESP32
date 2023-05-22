/*
MIT License

Copyright (c) 2023 esp32beans@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "switch_ESP32.h"
NSGamepad Gamepad;

void setup() {
  Gamepad.begin();
  USB.begin();
}

void loop() {
  static uint8_t count = NSButton_Y;
  if (count > NSButton_Capture) {
    Gamepad.releaseAll();
    count = NSButton_Y;
  }
  Gamepad.press(count);
  count++;

  // Move x/y Axis to a new position (16bit)
  Gamepad.leftXAxis(random(256));
  Gamepad.leftYAxis(random(256));
  Gamepad.rightXAxis(random(256));
  Gamepad.rightYAxis(random(256));

  // Go through all dPad positions
  static uint8_t dpad = NSGAMEPAD_DPAD_UP;
  Gamepad.dPad(dpad++);
  if (dpad > NSGAMEPAD_DPAD_UP_LEFT)
    dpad = NSGAMEPAD_DPAD_UP;

  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.loop();
  delay(100);
}
