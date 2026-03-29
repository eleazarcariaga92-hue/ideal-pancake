# ideal-pancake

ESP32-S3 firmware project using [PlatformIO](https://platformio.org/).

## Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) (`pio` on PATH)
- Python packages: `pip install pyserial esptool`

## Build

```bash
pio run -e esp32s3
```

Compiled firmware is placed at `.pio/build/esp32s3/firmware.bin`.

## Build & Upload (automated)

```bash
python scripts/auto_build_upload.py
```

The script attempts a PlatformIO upload first; if that fails it falls back to
flashing via `esptool.py` with automatic COM-port detection.