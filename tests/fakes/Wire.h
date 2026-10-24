#pragma once
#include <Arduino.h>
#include <set>
#include <vector>
class TwoWire {
public:
  std::set<uint8_t> devices;
  std::vector<uint8_t> bytes;
  unsigned long clock=0;
  bool started=false, failWrites=false;
  uint8_t target=0;
  void begin() { started=true; }
  void setClock(unsigned long hz) { clock=hz; }
  void beginTransmission(uint8_t address) { target=address; writing=false; }
  size_t write(uint8_t value) { writing=true; bytes.push_back(value); return 1; }
  uint8_t endTransmission() { return devices.count(target) && !(writing && failWrites) ? 0 : 2; }
private:
  bool writing=false;
};
extern TwoWire Wire;
