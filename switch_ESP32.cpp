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

#include <Arduino.h>

#if CONFIG_TINYUSB_HID_ENABLED

#include "switch_ESP32.h"

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
static const uint8_t report_descriptor[] = {
  // Gamepad for Nintendo Switch
  // 14 buttons, 1 8-way dpad, 2 analog sticks (4 axes)
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x35, 0x00,        //   Physical Minimum (0)
  0x45, 0x01,        //   Physical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x0E,        //   Report Count (14)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x0E,        //   Usage Maximum (0x0E)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,
                     //   Preferred State,No Null Position)
  0x95, 0x02,        //   Report Count (2)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,
                     //   Preferred State,No Null Position)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x25, 0x07,        //   Logical Maximum (7)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x09, 0x39,        //   Usage (Hat switch)
  0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,
                     //   Preferred State,Null State)
  0x65, 0x00,        //   Unit (None)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,
                     //   Preferred State,No Null Position)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x09, 0x30,        //   Usage (X)
  0x09, 0x31,        //   Usage (Y)
  0x09, 0x32,        //   Usage (Z)
  0x09, 0x35,        //   Usage (Rz)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x04,        //   Report Count (4)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,
                     //   Preferred State,No Null Position)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,
                     //   Preferred State,No Null Position)
  0xC0,              // End Collection
};

NSGamepad::NSGamepad(void) : hid() {
  static bool initialized = false;
  startMillis = 0;
  USB.VID(0x0f0d);
  USB.PID(0x00c1);
  USB.usbClass(0);
  USB.usbSubClass(0);
  USB.usbProtocol(0);
  end();
  if (!initialized) {
    initialized = true;
    hid.addDevice(this, sizeof(report_descriptor));
  }
}

uint16_t NSGamepad::_onGetDescriptor(uint8_t* dst) {
  memcpy(dst, report_descriptor, sizeof(report_descriptor));
  return sizeof(report_descriptor);
}

void NSGamepad::begin(void) {
  hid.begin();
  end();
}

void NSGamepad::end(void) {
  memset(&_report, 0x00, sizeof(_report));
  _report.leftXAxis = _report.leftYAxis = 0x80;
  _report.rightXAxis = _report.rightYAxis = 0x80;
  _report.dPad = NSGAMEPAD_DPAD_CENTERED;
}

bool NSGamepad::write(void) {
  return hid.SendReport(0, &_report, sizeof(_report));
}

bool NSGamepad::write(void *report, size_t len) {
  memcpy(&_report, report, min(len, sizeof(_report)));
  return write();
}

void NSGamepad::loop(void) {
  if (startMillis != millis()) {
    write();
    startMillis = millis();
  }
}

void NSGamepad::press(uint8_t b) {
  if (b > 15) b = 15;
  _report.buttons |= (uint16_t)1 << b;
}

void NSGamepad::release(uint8_t b) {
  if (b > 15) b = 15;
  _report.buttons &= ~((uint16_t)1 << b);
}

void NSGamepad::allAxes(uint32_t RYRXLYLX) {
  _report.rightYAxis = ((RYRXLYLX >> 24) & 0xFF) ^ 0x80;
  _report.rightXAxis = ((RYRXLYLX >> 16) & 0xFF) ^ 0x80;
  _report.leftYAxis  = ((RYRXLYLX >>  8) & 0xFF) ^ 0x80;
  _report.leftXAxis  = ((RYRXLYLX      ) & 0xFF) ^ 0x80;
}

void NSGamepad::allAxes(uint8_t RY, uint8_t RX, uint8_t LY, uint8_t LX) {
  _report.rightYAxis = RY ^ 0x80;
  _report.rightXAxis = RX ^ 0x80;
  _report.leftYAxis  = LY ^ 0x80;
  _report.leftXAxis  = LX ^ 0x80;
}

// The direction pad is limited to 8 directions plus centered. This means
// some combinations of 4 dpad buttons are not valid and cannot be sent.
// Button down = true(1)
// Valid: Any 1 button down, any 2 adjacent buttons down, no buttons down
// Invalid: all other combinations
void NSGamepad::dPad(bool up, bool down, bool left, bool right) {
  static const NSDirection_t BITS2DIR[16] = {
    NSGAMEPAD_DPAD_CENTERED,    // 0000
    NSGAMEPAD_DPAD_RIGHT,       // 0001
    NSGAMEPAD_DPAD_LEFT,        // 0010
    NSGAMEPAD_DPAD_CENTERED,    // 0011
    NSGAMEPAD_DPAD_DOWN,        // 0100
    NSGAMEPAD_DPAD_DOWN_RIGHT,  // 0101
    NSGAMEPAD_DPAD_DOWN_LEFT,   // 0110
    NSGAMEPAD_DPAD_CENTERED,    // 0111
    NSGAMEPAD_DPAD_UP,          // 1000
    NSGAMEPAD_DPAD_UP_RIGHT,    // 1001
    NSGAMEPAD_DPAD_UP_LEFT,     // 1010
    NSGAMEPAD_DPAD_CENTERED,    // 1011
    NSGAMEPAD_DPAD_CENTERED,    // 1100
    NSGAMEPAD_DPAD_CENTERED,    // 1101
    NSGAMEPAD_DPAD_CENTERED,    // 1110
    NSGAMEPAD_DPAD_CENTERED     // 1111
  };
  uint8_t dpad_bits = (up << 3) | (down << 2) | (left << 1) | (right << 0);
  _report.dPad = BITS2DIR[dpad_bits];
}

#endif /* CONFIG_TINYUSB_HID_ENABLED */
