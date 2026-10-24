#include <Robolark_LCD_PCF8574.h>

Robolark_LCD_PCF8574 lcd; // Auto scan 0x20..0x27. Or: lcd(0x27).

void setup() {
  Serial.begin(9600);
  if (!lcd.begin()) {
    Serial.print("LCD error code: ");
    Serial.println(static_cast<int>(lcd.status()));
    while (true) delay(1000);
  }
  lcd.printLine(0, "ROBOLARK");
  lcd.printLine(1, "LCD Working!");
}

void loop() {}
