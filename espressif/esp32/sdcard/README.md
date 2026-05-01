# ESP32 + microSD Card (SPI) Sample

Minimal ESP-IDF sample that mounts a microSD card over SPI, writes a file,
reads it back, and lists the root directory. Intended as a standalone test
rig for a Catalex-style microSD breakout when a larger project's SD code
isn't working.

## Hardware

- ESP32 dev board (classic ESP32, e.g. WROOM-32; **not** ESP32-S3 — pin
  numbers below are for classic ESP32).
- Catalex / generic microSD card module (SPI, 5V or 3V3).
- microSD card formatted as FAT32 (or exFAT if your IDF build supports it).

## Wiring

| SD module | ESP32 GPIO | Notes |
|-----------|------------|-------|
| `CS`      | **GPIO 16** | |
| `SCK`     | **GPIO 15** | |
| `MOSI`    | **GPIO 13** | |
| `MISO`    | **GPIO 12** | Strapping pin — must be LOW at boot. The Catalex module pulls MISO up via the SD card; if your board fails to boot, briefly remove the SD card during reset, or move MISO to a non-strapping GPIO. |
| `VCC`     | 5V          | Use 3V3 if your module is 3V3-only. |
| `GND`     | GND         | Common ground with the ESP32. |

Keep the jumper wires short (< ~15 cm). Long/unshielded wires are the most
common cause of intermittent SD failures.

## Build / Flash / Monitor

```powershell
# From an ESP-IDF PowerShell prompt:
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

Replace `COMx` with the port your board enumerates as.

## Expected Output

```
I (xxx) SDCARD_SAMPLE: Mounting SD card at /sdcard (MOSI=13 MISO=12 SCLK=15 CS=16, 4000 kHz)
Name: SD
Type: SDHC/SDXC
Speed: 20.00 MHz ...
Size: ... MB
I (xxx) SDCARD_SAMPLE: SD card mounted.
I (xxx) SDCARD_SAMPLE: Writing /sdcard/hello.txt ...
I (xxx) SDCARD_SAMPLE: Write OK.
I (xxx) SDCARD_SAMPLE: Reading /sdcard/hello.txt ...
I (xxx) SDCARD_SAMPLE:   Hello from ESP32 SD sample!
...
I (xxx) SDCARD_SAMPLE: Listing /sdcard
I (xxx) SDCARD_SAMPLE:   hello.txt                       58 bytes
I (xxx) SDCARD_SAMPLE: SD card unmounted.
I (xxx) SDCARD_SAMPLE: Done. Idling.
```

## Troubleshooting

If `Mount failed` appears:

1. **Lower the SPI clock.** In `main/main.c`, change:
   ```c
   #define SD_SPI_FREQ_KHZ 4000
   ```
   to `1000` (1 MHz) or `400`. Long jumper wires often need this.
2. **Double-check wiring**, especially that MOSI/MISO aren't swapped (this
   is the #1 cause of "mount failed" with a working card).
3. **Try another SD card.** Some very large or very old cards don't enumerate
   reliably in SPI mode. A 4–32 GB SDHC card formatted FAT32 is the safest bet.
4. **Power.** Brown-outs during card init look like SPI errors. If the
   module has a `5V` pin, prefer that over `3V3` (the on-board LDO can
   supply ~150 mA peak the SD card needs at startup).
5. **Strapping pin (MISO=GPIO 12).** If the board fails to boot at all
   (boot loop or "invalid header"), the SD card is pulling MISO high during
   reset. Eject the card, reset, then re-insert; or remap MISO to a
   non-strapping GPIO (e.g. GPIO 19) by editing `SD_PIN_MISO` in
   `main/main.c`.
6. **Format.** Use the SD Association's official formatter and choose
   FAT32. Cards formatted by Windows for sizes > 32 GB default to exFAT
   which requires `CONFIG_FATFS_LFN_MODE_HEAP=y` (already set) and IDF
   exFAT support; FAT32 always works.
