# ideal-pancake

ESP32-S3 firmware for the **Waveshare ESP32-S3-Touch-LCD-3.5** using [PlatformIO](https://platformio.org/).

## Hardware

| Component | Details |
|-----------|---------|
| MCU | ESP32-S3, 16 MB Flash, 8 MB OPI PSRAM |
| Display | ST7796 SPI IPS LCD, 480 × 320 px |
| Touch | FT6336 capacitive (I²C) |
| PMIC | AXP2101 (I²C) – controls LCD backlight |

## Pin Map (from official Waveshare schematic)

| Signal | GPIO |
|--------|------|
| SPI MOSI | 10 |
| SPI SCLK | 11 |
| SPI MISO | 12 |
| LCD CS | 13 |
| LCD DC | 15 |
| LCD RST | 17 |
| I²C SDA (touch + PMIC) | 8 |
| I²C SCL (touch + PMIC) | 7 |
| Touch INT | 3 |
| Touch RST | 2 |

## Requirements

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html) (`pio` on PATH)
- Library installed automatically by PlatformIO: `bodmer/TFT_eSPI`

## Build

```bash
pio run -e esp32s3
```

## Upload

```bash
pio run -e esp32s3 -t upload
```

Or use the automated helper script (auto-detects COM port, falls back to esptool).
Requires `pip install pyserial esptool` for the fallback path:

```bash
python scripts/auto_build_upload.py
```

## Monitor

```bash
pio device monitor -e esp32s3
```

## What the firmware does

1. Starts the AXP2101 PMIC over I²C and enables the LDO outputs that power the
   LCD backlight and touch controller.
2. Detects the FT6336 touch controller and resets it.
3. Initialises the ST7796 display in landscape (480 × 320) and shows a splash
   screen with colour swatches.
4. In the main loop, touch the screen to draw filled circles.  The colour
   auto-cycles through a palette each time you lift your finger.

## Notes

- If the display stays dark, confirm the board has an AXP2101 (check the IC
  marking).  A few early batches use a direct backlight GPIO instead.
- Touch coordinates are mapped from the FT6336 portrait-native frame to the
  landscape screen frame.  If touches feel mirrored or swapped, flip the sign
  of `sx`/`sy` in `loop()`.
