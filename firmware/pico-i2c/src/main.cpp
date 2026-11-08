// SW6206 I2C diagnosis master on a Pico. Bit-banged, open drain, internal pull-ups, ~10 kHz.
// SDA=GP4 (pin 6), SCL=GP5 (pin 7). Output on USB serial 115200.
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

static const int SDA_P = 4, SCL_P = 5;
static const unsigned HALF_US = 50;  // ~10 kHz

static void say(const char *fmt, ...) {
  char b[160]; va_list ap; va_start(ap, fmt); vsnprintf(b, sizeof b, fmt, ap); va_end(ap); Serial.print(b);
}
static inline void lo(int p) { pinMode(p, OUTPUT); digitalWrite(p, LOW); }
static inline void rel(int p) { pinMode(p, INPUT_PULLUP); }
static inline int rd(int p) { return digitalRead(p); }

static bool sclHigh() {  // release SCL, wait for clock stretching (max 2 ms)
  rel(SCL_P);
  for (int i = 0; i < 200; i++) { if (rd(SCL_P)) return true; delayMicroseconds(10); }
  return false;
}
static void start() { rel(SDA_P); sclHigh(); delayMicroseconds(HALF_US); lo(SDA_P); delayMicroseconds(HALF_US); lo(SCL_P); }
static void stop()  { lo(SDA_P); delayMicroseconds(HALF_US); sclHigh(); delayMicroseconds(HALF_US); rel(SDA_P); delayMicroseconds(HALF_US); }
static bool wbyte(uint8_t v) {  // returns true if ACK
  for (int i = 7; i >= 0; i--) {
    if (v & (1 << i)) rel(SDA_P); else lo(SDA_P);
    delayMicroseconds(HALF_US / 2); sclHigh(); delayMicroseconds(HALF_US); lo(SCL_P); delayMicroseconds(HALF_US / 2);
  }
  rel(SDA_P); delayMicroseconds(HALF_US / 2); sclHigh(); delayMicroseconds(HALF_US / 2);
  bool ack = !rd(SDA_P); delayMicroseconds(HALF_US / 2); lo(SCL_P); delayMicroseconds(HALF_US / 2);
  return ack;
}
static uint8_t rbyte(bool ack) {
  uint8_t v = 0; rel(SDA_P);
  for (int i = 0; i < 8; i++) {
    delayMicroseconds(HALF_US / 2); sclHigh(); delayMicroseconds(HALF_US / 2);
    v = (v << 1) | (rd(SDA_P) ? 1 : 0); delayMicroseconds(HALF_US / 2); lo(SCL_P); delayMicroseconds(HALF_US / 2);
  }
  if (ack) lo(SDA_P); else rel(SDA_P);
  delayMicroseconds(HALF_US / 2); sclHigh(); delayMicroseconds(HALF_US); lo(SCL_P); rel(SDA_P);
  return v;
}
static bool probe(uint8_t a7) { start(); bool a = wbyte(a7 << 1); stop(); return a; }
static bool readReg(uint8_t a7, uint8_t reg, uint8_t &out) {
  start(); if (!wbyte(a7 << 1)) { stop(); return false; }
  if (!wbyte(reg)) { stop(); return false; }
  start(); if (!wbyte((a7 << 1) | 1)) { stop(); return false; }
  out = rbyte(false); stop(); return true;
}
static void levels(const char *tag) {
  pinMode(SDA_P, INPUT); pinMode(SCL_P, INPUT); delay(2);
  int a = rd(SDA_P), c = rd(SCL_P);
  pinMode(SDA_P, INPUT_PULLDOWN); pinMode(SCL_P, INPUT_PULLDOWN); delay(2);
  int ad = rd(SDA_P), cd = rd(SCL_P);
  pinMode(SDA_P, INPUT_PULLUP); pinMode(SCL_P, INPUT_PULLUP); delay(2);
  int au = rd(SDA_P), cu = rd(SCL_P);
  say("%s levels  float SDA=%d SCL=%d | pulldown SDA=%d SCL=%d | pullup SDA=%d SCL=%d\n", tag, a, c, ad, cd, au, cu);
}

void setup() { Serial.begin(115200); delay(2000); Serial.println("--- SW6206 diag master (bit-bang 10 kHz, GP4=SDA GP5=SCL) ---"); }

void loop() {
  uint8_t a[256], b[256]; bool ok[256];
  for (int r = 0; r < 256; r++) { uint8_t v1 = 0, v2 = 0; bool o1 = readReg(0x3C, r, v1), o2 = readReg(0x3C, r, v2); a[r] = v1; b[r] = v2; ok[r] = o1 && o2; }
  Serial.println("--- reg dump 0x3C (AA=NAK, *=reads differ) ---");
  for (int r = 0; r < 256; r += 16) {
    say("%02X:", r);
    for (int i = 0; i < 16; i++) {
      int k = r + i;
      if (!ok[k]) say(" AA");  // NAK
      else say(" %02X%s", a[k], a[k] != b[k] ? "*" : "");
    }
    Serial.println();
  }
  delay(2000);
}
