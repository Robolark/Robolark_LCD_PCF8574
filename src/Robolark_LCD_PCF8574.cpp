// SPDX-License-Identifier: MIT
#include "Robolark_LCD_PCF8574.h"

Robolark_LCD_PCF8574::Robolark_LCD_PCF8574(uint8_t address, TwoWire &wire)
    : _wire(wire), _requestedAddress(address), _address(address),
      _columns(16), _rows(2), _displayControl(4), _cursorAddress(0),
      _pins{0, 1, 2, 3, 4, 5, 6, 7, true},
      _status(NOT_INITIALIZED), _ready(false), _backlight(true) {}

bool Robolark_LCD_PCF8574::validMap() const {
  const uint8_t pins[] = {_pins.rs, _pins.rw, _pins.enable, _pins.backlight,
                         _pins.d4, _pins.d5, _pins.d6, _pins.d7};
  uint8_t used = 0;
  for (uint8_t pin : pins) {
    if (pin > 7 || (used & (1U << pin))) return false;
    used |= (1U << pin);
  }
  return true;
}
bool Robolark_LCD_PCF8574::setPinMap(const PinMap &map) {
  if (_ready) { _status = INVALID_CONFIG; return false; }
  PinMap previous = _pins;
  _pins = map;
  if (!validMap()) { _pins = previous; _status = INVALID_CONFIG; return false; }
  return true;
}
bool Robolark_LCD_PCF8574::probe(uint8_t address) {
  _wire.beginTransmission(address);
  return _wire.endTransmission() == 0;
}
bool Robolark_LCD_PCF8574::begin(uint8_t columns, uint8_t rows, bool startWire) {
  _ready = false;
  _status = OK;
  // Restrict the documented geometry to one or two standard LCD rows.
  if (!columns || columns > 40 || !rows || rows > 2 || !validMap() ||
      (_requestedAddress && !((_requestedAddress >= 0x20 && _requestedAddress <= 0x27) ||
                             (_requestedAddress >= 0x38 && _requestedAddress <= 0x3F)))) {
    _status = INVALID_CONFIG; return false;
  }
  if (startWire) _wire.begin();
  _wire.setClock(100000);
  _address = _requestedAddress;
  if (!_address) {
    uint8_t count = 0;
    for (uint8_t a = 0x20; a <= 0x27; ++a) {
      if (probe(a)) { _address = a; ++count; }
    }
    if (count != 1) {
      _address = 0;
      _status = count ? MULTIPLE_DEVICES : NOT_FOUND;
      return false;
    }
  } else if (!probe(_address)) { _status = NOT_FOUND; return false; }
  _columns = columns;
  _rows = rows;
  _displayControl = 4;
  _cursorAddress = 0;
  _backlight = true;
  // HD44780 initialization by instruction, with conservative delays.
  delay(50);
  if (!output(0)) return false;
  if (!nibble(3, false)) return false;
  delay(5);
  if (!nibble(3, false)) return false;
  delayMicroseconds(150);
  if (!nibble(3, false)) return false;
  delayMicroseconds(150);
  if (!nibble(2, false)) return false;
  _ready = true;
  command(rows == 2 ? 0x28 : 0x20);
  command(0x08); // display off
  clear();
  command(0x06); // increment cursor, no automatic display shift
  updateDisplay();
  if (_status != OK) { _ready = false; return false; }
  return true;
}
bool Robolark_LCD_PCF8574::output(uint8_t value) {
  const uint8_t mask = (1U << _pins.backlight);
  value &= ~mask;
  if (_backlight == _pins.backlightActiveHigh) value |= mask;
  // All writes keep RW low; fixed delays replace busy-flag polling.
  _wire.beginTransmission(_address);
  _wire.write(value);
  if (_wire.endTransmission() != 0) { _status = I2C_ERROR; return false; }
  return true;
}
bool Robolark_LCD_PCF8574::nibble(uint8_t value, bool data) {
  const uint8_t pins[] = {_pins.d4, _pins.d5, _pins.d6, _pins.d7};
  uint8_t bits = data ? (1U << _pins.rs) : 0;
  for (uint8_t i = 0; i < 4; ++i)
    if (value & (1U << i)) bits |= (1U << pins[i]);
  if (!output(bits) || !output(bits | (1U << _pins.enable))) return false;
  delayMicroseconds(1);
  if (!output(bits)) return false;
  delayMicroseconds(50);
  return true;
}
bool Robolark_LCD_PCF8574::send(uint8_t value, bool data) {
  if (!_ready || _status != OK) return false;
  return nibble(value >> 4, data) && nibble(value & 0x0F, data);
}
void Robolark_LCD_PCF8574::command(uint8_t value) {
  if (send(value, false) && (value == 1 || value == 2)) delay(2);
}
void Robolark_LCD_PCF8574::clear() { command(1); _cursorAddress = 0; }
void Robolark_LCD_PCF8574::home() { command(2); _cursorAddress = 0; }
void Robolark_LCD_PCF8574::setCursor(uint8_t column, uint8_t row) {
  if (column >= _columns || row >= _rows) return;
  _cursorAddress = (row ? 0x40 : 0) + column;
  command(0x80 | _cursorAddress);
}
size_t Robolark_LCD_PCF8574::write(uint8_t value) {
  if (!send(value, true)) { setWriteError(); return 0; }
  _cursorAddress = (_cursorAddress + 1) & 0x7F;
  return 1;
}
void Robolark_LCD_PCF8574::backlight() {
  _backlight = true;
  if (_ready && _status == OK) output(0);
}
void Robolark_LCD_PCF8574::noBacklight() {
  _backlight = false;
  if (_ready && _status == OK) output(0);
}
void Robolark_LCD_PCF8574::updateDisplay() { command(0x08 | _displayControl); }
void Robolark_LCD_PCF8574::display() { _displayControl |= 4; updateDisplay(); }
void Robolark_LCD_PCF8574::noDisplay() { _displayControl &= ~4; updateDisplay(); }
void Robolark_LCD_PCF8574::cursor() { _displayControl |= 2; updateDisplay(); }
void Robolark_LCD_PCF8574::noCursor() { _displayControl &= ~2; updateDisplay(); }
void Robolark_LCD_PCF8574::blink() { _displayControl |= 1; updateDisplay(); }
void Robolark_LCD_PCF8574::noBlink() { _displayControl &= ~1; updateDisplay(); }
void Robolark_LCD_PCF8574::scrollDisplayLeft() { command(0x18); }
void Robolark_LCD_PCF8574::scrollDisplayRight() { command(0x1C); }
void Robolark_LCD_PCF8574::createChar(uint8_t slot, const uint8_t bitmap[8]) {
  if (slot > 7 || !bitmap) return;
  uint8_t saved = _cursorAddress;
  command(0x40 | (slot << 3));
  for (uint8_t i = 0; i < 8; ++i) write(bitmap[i] & 0x1F);
  command(0x80 | saved);
  _cursorAddress = saved;
}
void Robolark_LCD_PCF8574::printLine(uint8_t row, const char *text) {
  if (row >= _rows || !text) return;
  setCursor(0, row);
  bool ended = false;
  for (uint8_t i = 0; i < _columns; ++i) {
    if (!ended && !*text) ended = true;
    write(ended ? ' ' : static_cast<uint8_t>(*text++));
  }
}
