# ideal-pancake

ESP32-S3 firmware project using [PlatformIO](https://platformio.org/).

> **Looking for the website?**
> See [`index.html`](./index.html) for the modern luxury site design and
> [`WIX_GUIDE.md`](./WIX_GUIDE.md) for step-by-step instructions on applying the design
> inside the Wix Editor.

## Requirements

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html) (`pio` on PATH)

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