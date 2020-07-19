/*
Sample sketch for ADTB7M (https://bit-trade-one.co.jp/selfmadekb/adtb7m/)
Copyright 2020 kmgnkhr

SDIO -> D15
SCLK -> D14
TactSw -> D9, D16, D10
*/

#include "Mouse.h"

namespace {

void Pulse() {
  // between 1us and 12.5us.
  ::delayMicroseconds(5);
}

void Direction(uint8_t mode, uint8_t bit,
               volatile uint8_t* reg, volatile uint8_t* out) {
  switch (mode) {
    case OUTPUT:
      *reg |= bit;
      break;
    case INPUT:
      *reg &= ~bit;
      *out &= ~bit;
      break;
    case INPUT_PULLUP:
      *reg &= ~bit;
      *out |= bit;
      break;
  }
}

void WritePort(uint8_t val, uint8_t bit, volatile uint8_t* reg) {
  if (val == LOW) {
    *reg &= ~bit;
  } else {
    *reg |= bit;
  }
}

bool ReadPort(uint8_t bit, volatile const uint8_t* reg) {
  return (*reg & bit);
}

void SCLK_Direction(uint8_t mode) {
  Direction(mode, _BV(1), &DDRB, &PORTB);
}

void SCLK(uint8_t val, bool wait = true) {
  WritePort(val, _BV(1), &PORTB);
  if (wait) {
    Pulse();
  }
}

void SDIO_Direction(uint8_t mode) {
  Direction(mode, _BV(3), &DDRB, &PORTB);
}

void SDIO(uint8_t val) {
  WritePort(val, _BV(3), &PORTB);
}

bool SDIO() {
  return ReadPort(_BV(3), &PINB);
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
    if (SDIO()) {
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

  ::pinMode(9, INPUT_PULLUP);
  ::pinMode(16, INPUT_PULLUP);
  ::pinMode(10, INPUT_PULLUP);
}

namespace {

void ScanButton(uint8_t pin, uint8_t button) {
  if (::digitalRead(pin) == LOW) {
    if (!Mouse.isPressed(button)) {
      Mouse.press(button);
    }
  } else {
    if (Mouse.isPressed(button)) {
      Mouse.release(button);
    }
  }
}

}  // namespace

void loop() {
  auto st = ReadRegister(0x02);

  if (st & 0x80) {
    auto x = static_cast<int8_t>(ReadRegister(0x03));
    auto y = static_cast<int8_t>(ReadRegister(0x04));
    auto res = st & 0x7;
    auto dpi = DPI(res);

    if (::digitalRead(10) == LOW) {
      Mouse.move(0, 0, ConvertResolution(y, dpi, 150));
    } else {
      Mouse.move(ConvertResolution(x, dpi),
                ConvertResolution(y, dpi),
                0);
    }
  }

  ScanButton(9, MOUSE_LEFT);
  ScanButton(16, MOUSE_RIGHT);

  ::delay(2);
}
