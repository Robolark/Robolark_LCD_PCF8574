#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);
  delay(1000);
  Serial.println("Scanning I2C addresses...");
  uint8_t count = 0;
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Device at 0x");
      if (address < 16) Serial.print('0');
      Serial.println(address, HEX);
      ++count;
    }
  }
  Serial.print("Devices found: ");
  Serial.println(count);
}

void loop() {}
