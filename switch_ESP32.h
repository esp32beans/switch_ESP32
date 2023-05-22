/************************************************************************
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
*************************************************************************/
#ifndef SWITCH_ESP32_H_
#define SWITCH_ESP32_H_

#include "USB.h"
#include "USBHID.h"
#if CONFIG_TINYUSB_HID_ENABLED

// Dpad directions
typedef uint8_t NSDirection_t;
#define NSGAMEPAD_DPAD_UP  0
#define NSGAMEPAD_DPAD_UP_RIGHT 1
#define NSGAMEPAD_DPAD_RIGHT 2
#define NSGAMEPAD_DPAD_DOWN_RIGHT 3
#define NSGAMEPAD_DPAD_DOWN 4
#define NSGAMEPAD_DPAD_DOWN_LEFT 5
#define NSGAMEPAD_DPAD_LEFT 6
#define NSGAMEPAD_DPAD_UP_LEFT 7
#define NSGAMEPAD_DPAD_CENTERED 0xF

enum NSButtons {
  NSButton_Y = 0,
  NSButton_B,
  NSButton_A,
  NSButton_X,
  NSButton_LeftTrigger,
  NSButton_RightTrigger,
  NSButton_LeftThrottle,
  NSButton_RightThrottle,
  NSButton_Minus,
  NSButton_Plus,
  NSButton_LeftStick,
  NSButton_RightStick,
  NSButton_Home,
  NSButton_Capture,
  NSButton_Reserved1,
  NSButton_Reserved2
};

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

// 14 Buttons, 4 Axes, 1 D-Pad
typedef struct ATTRIBUTE_PACKED {
  uint16_t buttons;

  uint8_t dPad;

  uint8_t leftXAxis;
  uint8_t leftYAxis;

  uint8_t rightXAxis;
  uint8_t rightYAxis;
  uint8_t filler;
} HID_NSGamepadReport_Data_t;

class NSGamepad: public USBHIDDevice {
  public:
    NSGamepad(void);

    void begin(void);
    void end(void);
    void loop(void);
    bool write(void);
    bool write(void *report, size_t len);
    void press(uint8_t b);
    void release(uint8_t b);
    inline void releaseAll(void) { _report.buttons = 0; }
    inline void buttons(uint16_t b) { _report.buttons = b; }
    inline void leftXAxis(uint8_t a) { _report.leftXAxis = a; }
    inline void leftYAxis(uint8_t a) { _report.leftYAxis = a; }
    inline void rightXAxis(uint8_t a) { _report.rightXAxis = a; }
    inline void rightYAxis(uint8_t a) { _report.rightYAxis = a; }
    void allAxes(uint32_t RYRXLYLX);
    void allAxes(uint8_t RY, uint8_t RX, uint8_t LY, uint8_t LX);
    inline void dPad(NSDirection_t d) { _report.dPad = d; }
    void dPad(bool up, bool down, bool left, bool right);

    // internal use
    uint16_t _onGetDescriptor(uint8_t* buffer);
  protected:
    USBHID hid;
    HID_NSGamepadReport_Data_t _report;
    uint32_t startMillis;
};

#endif  // CONFIG_TINYUSB_HID_ENABLED
#endif  // SWITCH_ESP32_H_
