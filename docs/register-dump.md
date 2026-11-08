# Register dump of the SW6206S at address 0x3C

Master: Raspberry Pi Pico, bit-banged I2C at 10 kHz, state: battery connected, module idle. `AA` = the chip did not acknowledge in that read (the bus is disturbed now and then, so these positions move between dumps; they are not unreadable registers).

```
     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
00:  00 00 00 AA AA AA AA AA AA AA 00 00 00 00 00 80
10:  00 00 00 00 00 00 00 00 00 00 1C 00 84 20 20 00
20:  00 02 00 FF 00 A4 08 00 62 00 80 44 06 26 00 00
30:  21 20 68 04 10 00 00 48 00 00 00 00 00 00 00 00
40:  41 18 00 D0 3B 38 40 04 80 63 01 00 00 00 00 00
50:  00 00 00 00 00 00 00 42 1B 02 05 00 00 00 03 04
60:  3D 00 00 00 00 00 00 00 AA AA AA AA AA AA AA 00
70:  00 00 00 00 00 00 00 00 00 01 04 00 00 10 00 00
80:  00 00 06 10 18 22 2C 34 39 3E 42 46 4E 56 5D 62
90:  00 00 00 00 00 00 00 00 00 00 00 19 00 C0 9D 00
A0:  00 3C 00 00 00 00 00 00 00 00 00 00 E1 28 20 00
B0:  00 00 00 00 AA AA AA AA AA AA AA 00 00 00 00 00
C0:  1B A2 0E 84 80 2E 04 1A 11 01 6A C0 00 63 3D 04
D0:  0D 80 78 38 0A 80 04 80 00 81 04 A7 10 00 04 20
E0:  80 4C 57 08 00 0C 70 00 00 00 00 00 00 00 00 00
F0:  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```

## Observations (unconfirmed)

- `0x80` to `0x8F`: increasing table, maybe voltage to state of charge.
- `0x9E`: changes between `0x91` and `0x9D` over time.
- The meaning of every other register is unknown. Compare dumps with charger connected, with load and at different battery voltages, and note the measured battery voltage next to each dump.
