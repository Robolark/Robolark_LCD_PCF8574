#include <cassert>
#include <iostream>
#include <utility>
#include "Robolark_LCD_PCF8574.h"
TwoWire Wire;
using LCD = Robolark_LCD_PCF8574;
// Decode bytes captured at LCD enable-high edges for the default pin map.
std::vector<std::pair<bool,uint8_t>> decode(const TwoWire &wire) {
  std::vector<uint8_t> nibbles;
  for (uint8_t byte : wire.bytes) {
    assert((byte & 2) == 0); // Never request a read.
    if (byte & 4) nibbles.push_back(byte);
  }
  assert(nibbles.size()%2 == 0);
  std::vector<std::pair<bool,uint8_t>> result;
  for (size_t i=0;i<nibbles.size();i+=2) {
    assert((nibbles[i]&1)==(nibbles[i+1]&1));
    result.push_back({bool(nibbles[i]&1),uint8_t((nibbles[i]&0xF0)|(nibbles[i+1]>>4))});
  }
  return result;
}
int main() {
  TwoWire bus;
  LCD lcd(0,bus);
  assert(lcd.write('A')==0);
  assert(!lcd.begin()); assert(lcd.status()==LCD::NOT_FOUND);
  bus.devices={0x22,0x27};
  assert(!lcd.begin()); assert(lcd.status()==LCD::MULTIPLE_DEVICES);
  assert(lcd.address()==0); assert(bus.bytes.empty());
  bus.devices={0x27};
  assert(lcd.begin()); assert(lcd.address()==0x27);
  assert(bus.started && bus.clock==100000);
  // Verify the initialization handshake independently of full-byte decoding.
  std::vector<uint8_t> init;
  for (uint8_t byte : bus.bytes) if(byte&4) init.push_back(byte>>4);
  assert(init.size()>4);
  assert(init[0]==3 && init[1]==3 && init[2]==3 && init[3]==2);
  bus.bytes.clear();
  lcd.printLine(1,"Hi");
  auto data=decode(bus);
  assert(data.size()==17 && !data[0].first && data[0].second==0xC0);
  assert(data[1].first && data[1].second=='H' && data[2].second=='i');
  for(size_t i=3;i<data.size();++i) assert(data[i].second==' ');
  bus.bytes.clear();
  lcd.printLine(0,"12345678901234567890");
  data=decode(bus); assert(data.size()==17 && data.back().second=='6');
  bus.bytes.clear();
  lcd.setCursor(16,0); lcd.setCursor(0,2); lcd.printLine(2,"No");
  assert(bus.bytes.empty());
  lcd.setCursor(3,1); bus.bytes.clear();
  const uint8_t heart[8]={0,10,31,31,14,4,0,0};
  lcd.createChar(0,heart); data=decode(bus);
  assert(data.size()==10 && data.front().second==0x40 && data.back().second==0xC3);
  for(size_t i=0;i<8;++i) assert(data[i+1].first && data[i+1].second==heart[i]);
  bus.bytes.clear(); lcd.noBacklight(); assert(bus.bytes.back()==0);
  lcd.backlight(); assert(bus.bytes.back()==8);
  bus.failWrites=true;
  assert(lcd.write('A')==0); assert(lcd.status()==LCD::I2C_ERROR);
  bus.failWrites=false;
  assert(lcd.begin());
  TwoWire other; other.devices={0x27}; other.failWrites=true;
  LCD broken(0x27,other);
  assert(!broken.begin()); assert(broken.status()==LCD::I2C_ERROR);
  LCD invalid(0x10,other);
  assert(!invalid.begin()); assert(invalid.status()==LCD::INVALID_CONFIG);
  LCD configured(0x27,other);
  assert(!configured.begin(0,2));
  LCD::PinMap bad={0,0,2,3,4,5,6,7,true};
  assert(!configured.setPinMap(bad));
  other.failWrites=false;
  LCD::PinMap inverse={0,1,2,3,4,5,6,7,false};
  assert(configured.setPinMap(inverse));
  other.started=false;
  assert(configured.begin(16,2,false)); assert(!other.started);
  other.bytes.clear(); configured.noBacklight(); assert(other.bytes.back()==8);
  configured.backlight(); assert(other.bytes.back()==0);
  TwoWire aBus; aBus.devices={0x3F};
  LCD variant(0x3F,aBus); assert(variant.begin());
  std::cout << "All host tests passed\n";
}
