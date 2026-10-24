# ROBOLARK LCD PCF8574

Arduino library for HD44780-compatible character LCDs connected through a
PCF8574/PCF8574T I2C expander. Defaults to 16 columns and 2 rows.
Designed for Arduino-compatible cores with the standard Wire and Print APIs,
including AVR, ESP32, ESP8266, RP2040 (Arduino-Pico), and STM32duino. This is a
common **LCD driver**, not a general library for every sensor or programming
environment. Bare-metal 8051, MicroPython, ESP-IDF-only, and desktop Raspberry Pi
programs require separate ports.

Version 0.1.0 is a development release: host tests pass, but the physical
RG1602A-IIC(P) module and Arduino target builds have not yet been verified.

## Install

Download the supplied installable ZIP (or use GitHub **Code > Download ZIP**
after the source is uploaded), then in Arduino IDE select **Sketch > Include Library >
Add .ZIP Library**. The supplied ZIP contains one `Robolark_LCD_PCF8574` folder.
This library is not yet listed in Arduino Library Manager.
No external library dependencies; uses the Arduino core's Wire and Print classes.

## Connections: Arduino Uno R3 / classic Nano

Disconnect power before wiring.

| LCD | Arduino |
| --- | --- |
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

Follow the pin labels on the LCD; connector order varies. Adjust the onboard
contrast trimmer (RX1 on the photographed RG1602A board) slowly if text is faint.
The LCD backlight can light even when communication fails.

## Quick start

```cpp
#include <Robolark_LCD_PCF8574.h>

Robolark_LCD_PCF8574 lcd;

void setup() {
  Serial.begin(9600);
  if (!lcd.begin()) {
    Serial.print("LCD error: ");
    Serial.println(static_cast<int>(lcd.status()));
    while (true) delay(1000);
  }
  lcd.printLine(0, "ROBOLARK");
  lcd.printLine(1, "LCD Working!");
}
void loop() {}
```

Open **File > Examples > Robolark_LCD_PCF8574 > HelloRobolark** after installation.
Use the included `I2CScanner` example to inspect addresses.

## Address and pin mapping

The default constructor scans **0x20 through 0x27**, the PCF8574 address range.
It proceeds only if exactly one address acknowledges; no LCD commands are sent
when several candidates are present. Discovery detects an I2C acknowledgment,
not the chip type, LCD wiring, or whether characters are visible. If another
kind of device occupies this range, use an explicit known LCD address instead.

```cpp
Robolark_LCD_PCF8574 lcd(0x27);
```

PCF8574A addresses 0x38 through 0x3F require an explicit address. The `T` suffix
does not change the PCF8574 address range.

Default expander mapping:

| Expander pin | LCD signal |
| --- | --- |
| P0 | RS |
| P1 | RW |
| P2 | Enable |
| P3 | Backlight, active high |
| P4 | D4 |
| P5 | D5 |
| P6 | D6 |
| P7 | D7 |

This common mapping is an assumption for the photographed board, pending
hardware testing. Pin mapping is configurable, not automatically detected:

```cpp
Robolark_LCD_PCF8574::PinMap map = {0, 1, 2, 3, 4, 5, 6, 7, true};
lcd.setPinMap(map); // before begin(); all eight pins must be unique, 0..7
```

## API

| Call | Purpose |
| --- | --- |
| `begin()` | Initialize 16x2 LCD; returns true on acknowledged initialization writes |
| `begin(columns, rows)` | Set geometry, 1-40 columns and 1-2 rows |
| `address()` | Resolved 7-bit I2C address |
| `status()` | Last initialization error or sticky runtime I2C error |
| `print(...)` | Arduino Print formatting for text and numbers |
| `printLine(row, text)` | Truncate/pad a C string to the visible line width |
| `setCursor(column, row)` | Move cursor; zero-based; out-of-range requests ignored |
| `clear()` / `home()` | Clear display / return to origin |
| `backlight()` / `noBacklight()` | Switch backlight |
| `display()` / `noDisplay()` | Show/hide display content |
| `cursor()` / `noCursor()` | Show/hide underline cursor |
| `blink()` / `noBlink()` | Enable/disable blinking cursor |
| `scrollDisplayLeft()` / `scrollDisplayRight()` | Shift visible display |
| `createChar(slot, bitmap)` | Define one of eight 5x8 characters; restores DDRAM cursor |
| `write(uint8_t(0))` | Display custom character in slot 0 |

`printLine` is intended for normal, unshifted, left-to-right display use. Call
`home()` to undo scrolling before using it. `print()` does not wrap visible
lines or translate newlines; use `setCursor` to select the next row.
ASCII text and the controller's built-in character ROM are supported, not UTF-8.
4-row displays and unusual internally split 16x1 modules are outside this release.

### Errors

| Code | Meaning |
| --- | --- |
| 0 / OK | No recorded error |
| 1 / NOT_FOUND | No responding device at the selected address/range |
| 2 / MULTIPLE_DEVICES | More than one candidate; specify the LCD address |
| 3 / INVALID_CONFIG | Invalid geometry, address, or pin mapping |
| 4 / I2C_ERROR | An expander write failed |
| 5 / NOT_INITIALIZED | `begin()` has not succeeded |

Runtime I2C errors stop subsequent command/data writes. Fix the wiring or power
and call `begin()` again to reset status and reinitialize. `Print::getWriteError()`
is separate; use `clearWriteError()` to clear that flag in the real Arduino core.
Successful I2C writes cannot prove that LCD initialization or rendering succeeded.
A stuck bus may block according to the selected Arduino core's Wire implementation.

### Other boards and shared buses

The source uses portable Arduino APIs (`architectures=*`); this is not a claim
that every board has been tested. `begin()` sets the selected bus to **100 kHz**.
For a preconfigured bus, call `Wire.begin(...)` with your board-specific pins,
then `lcd.begin(16, 2, false)`. You can supply a different `TwoWire` instance as
the constructor's second argument. All devices on a shared bus must support
100 kHz. ESP32 GPIOs require suitable I2C level shifting if the LCD board pulls
SDA/SCL to 5V.

## Validation

Run `bash tests/run.sh` with g++. Host tests cover address selection, initialization
nibbles, decoded text/command bytes, line padding and truncation, bounds, CGRAM
cursor restoration, backlight polarity, failed writes, reinitialization, and
configuration validation. They simulate I2C acknowledgments and do not establish
physical timing, electrical compatibility, or the actual module's pin mapping.

GitHub Actions is configured to run host tests and compile all examples for
Uno, classic Nano, Mega, ESP32, ESP32-C3, ESP32-S3, ESP8266, RP2040 and STM32. Those Arduino builds have not run in this workspace.
Before a stable release, run CI and upload HelloRobolark on the physical module;
verify both lines, contrast, custom characters, backlight, and power cycling.

## Development and publishing

Intended repository: [Robolark/Robolark_LCD_PCF8574](https://github.com/Robolark/Robolark_LCD_PCF8574).
Upload this folder's **contents** to the repository root, so `library.properties`
and `src/` are at the top level. A step-by-step guide is in [SETUP.md](SETUP.md).
Run CI and test the actual module before a stable release. GitHub hosting does
not automatically add a library to Arduino Library Manager.

## Technical references

Original driver implementation using the documented HD44780 4-bit instruction
protocol, PCF8574 expander writes, and Arduino's Print API. No third-party driver
source is bundled.

- [Hitachi HD44780U datasheet](https://cdn-shop.adafruit.com/datasheets/HD44780.pdf)
- [NXP PCF8574 / PCF8574A](https://www.nxp.com/products/PCF8574P)
- [Arduino library format](https://docs.arduino.cc/arduino-cli/library-specification/)

MIT license, copyright 2026 ROBOLARK. [robolark.com](https://robolark.com)

### Board-specific examples and references

Use **File > Examples > Robolark_LCD_PCF8574 > CrossBoard** for explicit bus setup.
The pin numbers are documented example choices, not a universal board pinout.

- [ESP32 I2C API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html)
- [ESP8266 Wire API](https://github.com/esp8266/Arduino/blob/master/doc/libraries.rst)
- [Arduino-Pico Wire API](https://arduino-pico.readthedocs.io/en/latest/wire.html)
- [STM32duino I2C API](https://stm32duino.github.io/dev/libraries/built-in/I2C-library/)
