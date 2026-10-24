# Step-by-step setup

The library is published at [Robolark/Robolark_LCD_PCF8574](https://github.com/Robolark/Robolark_LCD_PCF8574).
Host tests and compilation for nine board targets passed. Physical LCD testing
is still pending.

## 1. Install in Arduino IDE

1. Download the supplied library ZIP, or GitHub **Code > Download ZIP**.
2. Open Arduino IDE.
3. Choose **Sketch > Include Library > Add .ZIP Library**.
4. Select the ZIP and wait for the installed message.
5. Open **File > Examples > Robolark_LCD_PCF8574 > HelloRobolark**.

For non-AVR boards, also install the appropriate board package in Boards Manager
using its official installation instructions. The library does not install a
board core, USB driver, or upload tool for you.

## 2. Wire the LCD

Disconnect power first. For Uno R3 or classic Nano, connect LCD VCC to 5V,
GND to GND, SDA to A4, and SCL to A5.

| Board / example | SDA | SCL | I2C logic |
| --- | --- | --- | --- |
| Uno R3 / classic Nano | A4 | A5 | 5V |
| Mega 2560 | 20 | 21 | 5V |
| Classic ESP32 DevKit, typical defaults | GPIO21 | GPIO22 | 3.3V |
| ESP32-C3, CrossBoard example choice | GPIO1 | GPIO2 | 3.3V |
| ESP32-S3 | Selected board's SDA | Selected board's SCL | 3.3V |
| NodeMCU ESP8266 | D2 / GPIO4 | D1 / GPIO5 | 3.3V |
| Pico RP2040, Arduino-Pico CrossBoard example | GP0 | GP1 | 3.3V |
| STM32duino | Selected board's SDA | Selected board's SCL | Board-specific |

**For 3.3V controllers with this 5V LCD:** use a bidirectional I2C level shifter.
Connect controller SDA/SCL to the low-voltage side, LCD SDA/SCL to the high-voltage
side, LV to 3.3V, HV to 5V and all grounds together. Supply LCD VCC with 5V.
Use a suitable regulated 5V source; verify the selected controller board's USB,
5V and VIN pin behavior before using any of them as a power source.
Do not connect LCD SDA/SCL lines pulled up to 5V directly to 3.3V-only GPIOs.
Powering the entire LCD at 3.3V is not a guaranteed substitute for level shifting.

Verify pin labels on your specific board and LCD, rather than connector order.

## 3. Select the board and upload

1. In Arduino IDE choose **Tools > Board** and your exact board model.
2. Choose **Tools > Port** and the connected board's port.
3. Use **HelloRobolark** for default I2C pins; use **CrossBoard** for explicit pins.
4. For CrossBoard, check the pin choices against the table and your board pinout.
5. Click **Verify**, then **Upload**.
6. Look for `ROBOLARK` on line 1 and `LCD Working!` on line 2 in HelloRobolark.
7. Slowly adjust RX1 if characters are invisible or show as solid blocks.

## 4. Troubleshoot

Open Serial Monitor at **9600 baud** to read an error code. See the README's
error table. Run **I2CScanner** using the same bus/pins as your LCD sketch (add the
same board-specific Wire setup from CrossBoard when custom pins are in use).
If the scan returns multiple candidates, specify your LCD's confirmed address:

```cpp
Robolark_LCD_PCF8574 lcd(0x27); // replace with your scanned address
```

The default expander-to-LCD mapping is common but unverified for the photographed
module. An ACK confirms the expander responds, not that this mapping is correct.
If contrast and wiring are correct but text still fails, verify the module's
mapping using its schematic or continuity measurements with power disconnected.

## 5. Release after hardware testing

Test both lines, clear, cursor movement, custom characters, backlight switching
and power cycling on the actual module. Record each tested board/core version.
Then tag and publish a release with the installable ZIP. Arduino Library Manager
registration is a separate process; GitHub upload alone does not register it.
