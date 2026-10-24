#include <Robolark_LCD_PCF8574.h>

Robolark_LCD_PCF8574 lcd;

void setup() {
  Serial.begin(9600);

  // Pin choices are examples. Check your exact board's pinout first.
#if defined(ARDUINO_ARCH_ESP32)
  #if defined(CONFIG_IDF_TARGET_ESP32C3)
    Wire.begin(1, 2); // ESP32-C3 example: SDA=GPIO1, SCL=GPIO2
  #else
    Wire.begin(SDA, SCL); // Board-defined defaults, including ESP32-S3
  #endif
#elif defined(ESP8266)
  Wire.begin(4, 5); // NodeMCU: SDA=D2/GPIO4, SCL=D1/GPIO5
#elif defined(ARDUINO_ARCH_RP2040)
  // Earle Philhower Arduino-Pico core, Wire/I2C0.
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();
#elif defined(ARDUINO_ARCH_STM32)
  // STM32duino: uses the selected board variant's SDA and SCL.
  // To override, call Wire.setSDA(...) and Wire.setSCL(...) before begin.
  Wire.begin();
#else
  // AVR Uno/Nano/Mega and other cores: board-defined default I2C pins.
  Wire.begin();
#endif

  // false preserves the already configured bus/pins above.
  if (!lcd.begin(16, 2, false)) {
    Serial.print("LCD error code: ");
    Serial.println(static_cast<int>(lcd.status()));
    while (true) delay(1000);
  }
  lcd.printLine(0, "ROBOLARK");
  lcd.printLine(1, "Common LCD Demo");
}

void loop() {}
