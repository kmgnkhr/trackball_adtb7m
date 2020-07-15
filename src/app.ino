/*
Sample sketch for ADTB7M (https://bit-trade-one.co.jp/selfmadekb/adtb7m/)
Copyright 2020 kmgnkhr

SDIO -> D10
SCLK -> D16
*/

#include "Mouse.h"

namespace {

void Pulse() {
  // between 1us and 12.5us.
  ::delayMicroseconds(5);
}

void SCLK_Direction(uint8_t mode) {
  switch (mode) {
    case OUTPUT:
      DDRB |= _BV(2);
      break;
    case INPUT:
      // DDRB &= ~_BV(2);
      // PORTB &= ~_BV(2);
      break;
  }
}

void SCLK(uint8_t val, bool wait = true) {
  if (val == LOW) {
    PORTB &= ~_BV(2);
  } else {
    PORTB |= _BV(2);
  }
  if (wait) {
    Pulse();
  }
}

void SDIO_Direction(uint8_t mode) {
  switch (mode) {
    case OUTPUT:
      DDRB |= _BV(6);
      break;
    case INPUT:
      DDRB &= ~_BV(6);
      PORTB &= ~_BV(6);
      break;
  }
}

void SDIO(uint8_t val) {
  if (val == LOW) {
    PORTB &= ~_BV(6);
  } else {
    PORTB |= _BV(6);
  }
}

uint8_t SDIO() {
  return (PINB & _BV(6)) ? HIGH : LOW;
}

void SetupSerial() {
  SCLK_Direction(OUTPUT);
  SDIO_Direction(OUTPUT);
  SCLK(HIGH);
}

void WriteByte(uint8_t d) {
  SDIO_Direction(OUTPUT);

  for (auto b = 0x80; b != 0; b >>= 1) {
    SCLK(LOW, false);
    SDIO(((d & b) == 0) ? LOW : HIGH);
    Pulse();
    SCLK(HIGH);
  }
}

uint8_t ReadRegister(int8_t address) {
  WriteByte(address & ~0x80);

  SDIO_Direction(INPUT);

  auto rx = 0;
  for (auto b = 0x80; b != 0; b >>= 1) {
    SCLK(LOW);
    if (SDIO() == HIGH) {
      rx |= b;
    }
    SCLK(HIGH);
  }
  return rx;
}

int DPI(int res) {
  static const int d[] = { 500, 650, 750, 1000, 1300, 1500 };
  if (res < 0 || res >= 6) {
    res = 2;
  }
  return d[res];
}

int ConvertResolution(int d, int src, int dst = 1300) {
  return static_cast<int>(d * dst / src);
}

}  // namespace

void setup() {
  Serial.begin(115200);
  SetupSerial();
  Mouse.begin();
}

void loop() {
  auto st = ReadRegister(0x02);

  if (st & 0x80) {
    auto x = static_cast<int8_t>(ReadRegister(0x03));
    auto y = static_cast<int8_t>(ReadRegister(0x04));
    auto res = st & 0x7;
    auto dpi = DPI(res);

    Mouse.move(ConvertResolution(x, dpi),
               ConvertResolution(y, dpi),
               0);
  }
  ::delay(2);
}
