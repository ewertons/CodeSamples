# ESP32 + Catalex microSD Module — Working Setup Notes

End-to-end notes for getting a Catalex-style microSD breakout working with
a classic ESP32 over SPI in ESP-IDF. Captures every gotcha hit during the
initial bring-up so future-me doesn't have to rediscover them.

Confirmed working with a SanDisk 32 GB SDHC card (`SD32G`).

## Final working configuration

### Hardware

| SD module pin | ESP32 GPIO | Notes |
|---------------|------------|-------|
| `CS`          | GPIO 16    | |
| `SCK`         | GPIO 15    | |
| `MOSI`        | GPIO 13    | |
| `MISO`        | GPIO 12    | Strapping pin — must be LOW at boot. If boot loops, eject the card during reset. |
| `VCC`         | **5V from USB rail (NOT the ESP32 `3V3` pin)** | This was the single biggest gotcha — see below. |
| `GND`         | GND        | Common ground with the ESP32. |

Keep jumper wires short (< ~15 cm). Long wires cause CRC errors at higher
SPI clock speeds.

### Software (sample at `codesamples/espressif/esp32/sdcard/`)

Key settings in `main/main.c`:

- `SD_SPI_FREQ_KHZ = 400` (slowest/safest; bumpable to 4000 once stable).
- `mount_cfg.format_if_mount_failed = true` (auto-recovers from a bad FS).
- Internal pull-ups enabled on MISO/CS/MOSI.
- Write path uses `fflush` + `fsync` + checked `fclose` so silent write
  failures are caught.

## Issues encountered (in order) and how they were fixed

### 1. Powering the module from the ESP32 `3V3` pin → `ESP_ERR_TIMEOUT (0x107)`

**Symptom:** `sdmmc_card_init failed (0x107)`. The card never replies to
the SPI init commands at all.

**Cause:** The ESP32 dev-board's on-board 3V3 LDO can't supply the inrush
current an SD card needs during init (~100–200 mA peak). The rail browns
out and the card never enumerates.

**Fix:** Power the Catalex module's `VCC` from **5V directly off the USB
cable rail**, not from the ESP32's `3V3` output pin. Keep a common GND.
Adding a 10 µF cap across VCC/GND right at the module is also a good idea
but wasn't required here.

> An `ESP_ERR_TIMEOUT` from `sdmmc_card_init` is **never** a filesystem
> issue — the card never identified. It's wiring, power, or a dead card.
> Don't waste time formatting in this state.

### 2. Card listed correctly but writes silently failed

**Symptom:** First card was a Raspberry Pi NOOBS recovery card. Mount
worked, listing showed all the recovery files, but `fopen("…", "w")`
appeared to succeed and `fclose` was not checked, so the test file
"vanished" between write and read-back.

**Cause:** The test harness ignored return values from `fprintf`/`fclose`.
The actual write was failing (likely on the SD card's read-only flag /
attributes from the Pi recovery image).

**Fix in code:**
- Check return values from `fprintf`, `fflush`, `fsync`, `fclose`.
- Log `errno` + `strerror(errno)` on each failure.

This made the next failure mode (read-only / wrong filesystem) visible
instead of silent.

### 3. Trying to format a 32 GB card from Windows Disk Management

**Symptom:** Disk Management greyed out "Delete Volume" on the EFI System
Partition that came pre-installed on the new card; could not get a clean
FAT32 layout that ESP-IDF would accept.

**Cause:** Windows Disk Management refuses to delete EFI System
partitions and several OEM partition types as a safety measure.

**Fix:** Use `diskpart` from an **elevated** PowerShell:

```text
diskpart
list disk
select disk N            # ← identify the SD card by SIZE; double-check!
clean
convert mbr              # ← critical: ESP-IDF FATFS only reads MBR, not GPT
create partition primary
list partition
select partition 1
format fs=fat32 quick label="SDCARD"
assign
exit
```

> A "32 GB" card showing as ~28.86 GB in Windows is normal. Manufacturers
> use decimal GB (32 × 10⁹ bytes); Windows reports binary GiB; SD
> controllers also reserve some sectors. ~28–30 GiB usable for a 32 GB
> card is expected.

### 4. Card formatted but ESP-IDF still gave FATFS error 13 (`FR_NO_FILESYSTEM`)

**Symptom:** `failed to mount card (13)` even though Windows mounted the
card fine and showed FAT32.

**Cause:** Cards previously partitioned as GPT (e.g. ones that had an EFI
System Partition) stay GPT after `clean` + `create partition primary`
unless `convert mbr` is issued explicitly. ESP-IDF's FATFS layer only
understands MBR partition tables, not GPT.

**Fix:** Add `convert mbr` to the `diskpart` sequence above. With an MBR
+ FAT32 layout the card mounts immediately on the ESP32.

### 5. Stale firmware confusing diagnosis

**Symptom:** `--- Warning: Checksum mismatch between flashed and built
applications` in the monitor output, plus old log strings appearing that
no longer exist in the source.

**Cause:** Running `idf.py -p COMx monitor` after editing code without
re-flashing. The chip keeps running the previous binary.

**Fix:** Always run `idf.py -p COMx flash monitor` after code changes
(not just `monitor`). If the checksum-mismatch warning appears, that's
the giveaway.

### 6. Re-seating the card produced `ESP_ERR_INVALID_CRC (0x108)`

**Symptom:** `sdmmc_read_sectors_dma … 0x108`. Card identified, but the
first sector read returned a bad CRC.

**Cause:** Flaky physical contact (microSD-to-SD adapter contacts, slight
mis-insertion, oxidation).

**Fix:** Eject, gently clean contacts, fully re-seat. Tried the same
card again with no other changes and it worked.

> `0x108` = signal-integrity / contact issue, not format.
> `0x107` = no response (power/wiring).
> FATFS `(13)` = no filesystem (or wrong partition table style).
> FATFS `(1)` = generic disk read error (often misreported as "no FS").

## Reproducible "from scratch" recipe

For when this needs to be done again:

1. **Wire** the module per the table above. **VCC must be on USB 5V**,
   not the ESP32 3V3 pin.
2. **Format the SD card** via `diskpart` with `clean → convert mbr →
   create partition primary → format fs=fat32 quick → assign`. Do NOT
   trust Windows Disk Management on a card that came with an EFI/OEM
   partition.
3. **Build & flash** the sample with `idf.py set-target esp32 && idf.py
   -p COMx flash monitor`.
4. Expect the log to show: `Name: …`, `SD card mounted.`, `Write OK`,
   `Hello from ESP32 SD sample!` echoed back, and a directory listing
   that includes `hello.txt`.
5. Once stable at 400 kHz, raise `SD_SPI_FREQ_KHZ` to `4000` (4 MHz) for
   normal use, or `20000` for max throughput. Drop back if CRC errors
   reappear.

## Reference: known-good output

```
I (308) SDCARD_SAMPLE: Mounting SD card at /sdcard (MOSI=13 MISO=12 SCLK=15 CS=16, 400 kHz)
Name: SD32G
Type: SDHC
Speed: 400.00 kHz (limit: 400.00 kHz)
Size: 29557MB
CSD: ver=2, sector_size=512, capacity=60532736 read_bl_len=9
SSR: bus_width=1
I (424) SDCARD_SAMPLE: SD card mounted.
I (428) SDCARD_SAMPLE: Writing /sdcard/hello.txt …
I (599) SDCARD_SAMPLE: Write OK (53 bytes).
I (599) SDCARD_SAMPLE: Reading /sdcard/hello.txt …
I (624) SDCARD_SAMPLE:   Hello from ESP32 SD sample!
I (624) SDCARD_SAMPLE:   Tick count at write: 212
I (625) SDCARD_SAMPLE: Listing /sdcard
I (627) SDCARD_SAMPLE:   System Volume Information        0 bytes (dir)
I (632) SDCARD_SAMPLE:   hello.txt                        53 bytes
I (639) SDCARD_SAMPLE: SD card unmounted.
I (642) SDCARD_SAMPLE: Done. Idling.
```
