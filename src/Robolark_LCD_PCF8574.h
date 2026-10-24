// SPDX-License-Identifier: MIT
#pragma once
#include <Arduino.h>
#include <Print.h>
#include <Wire.h>

class Robolark_LCD_PCF8574 : public Print {
public:
  // P0=RS, P1=RW, P2=E, P3=backlight, P4..P7=D4..D7 by default.
  struct PinMap {
    uint8_t rs, rw, enable, backlight, d4, d5, d6, d7;
    bool backlightActiveHigh;
  };
  enum Status : uint8_t {
    OK, NOT_FOUND, MULTIPLE_DEVICES, INVALID_CONFIG, I2C_ERROR, NOT_INITIALIZED
  };
  // Address 0 scans PCF8574 addresses 0x20..0x27 only.
  explicit Robolark_LCD_PCF8574(uint8_t address = 0, TwoWire &wire = Wire);
  // Configure custom buses with Wire.begin(...) first, then pass startWire=false.
  bool begin(uint8_t columns = 16, uint8_t rows = 2, bool startWire = true);
  bool setPinMap(const PinMap &map); // Call before begin().
  uint8_t address() const { return _address; }
  Status status() const { return _status; }
  void clear();
  void home();
  void setCursor(uint8_t column, uint8_t row);
  void backlight();
  void noBacklight();
  void display();
  void noDisplay();
  void cursor();
  void noCursor();
  void blink();
  void noBlink();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void createChar(uint8_t slot, const uint8_t bitmap[8]);
  // Truncates long text and pads short text to erase the previous line.
  // Intended for normal left-to-right use without display scrolling.
  void printLine(uint8_t row, const char *text);
  using Print::write;
  size_t write(uint8_t value) override;
private:
  TwoWire &_wire;
  uint8_t _requestedAddress, _address, _columns, _rows, _displayControl;
  uint8_t _cursorAddress;
  PinMap _pins;
  Status _status;
  bool _ready, _backlight;
  bool validMap() const;
  bool probe(uint8_t address);
  bool output(uint8_t value);
  bool nibble(uint8_t value, bool data);
  bool send(uint8_t value, bool data);
  void command(uint8_t value);
  void updateDisplay();
};
