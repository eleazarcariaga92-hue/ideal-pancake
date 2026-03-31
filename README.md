# ideal-pancake

ESP32-S3 firmware project using [PlatformIO](https://platformio.org/).

---

## Step-by-Step Guide

### Step 1 — Install Prerequisites

1. **Python 3.7+** — download from https://www.python.org/downloads/ and make sure to check *"Add Python to PATH"* during installation.
2. **PlatformIO Core** — after Python is installed, run:
   ```bash
   pip install platformio
   ```
   Verify with:
   ```bash
   pio --version
   ```
3. *(Optional — for the automated helper script)* Install `pyserial` and `esptool`:
   ```bash
   pip install pyserial esptool
   ```

---

### Step 2 — Open the Project

Clone or download this repository, then open a terminal (Command Prompt, PowerShell, or bash) in the project root folder — the folder that contains `platformio.ini`.

---

### Step 3 — Understand the Project Structure

```
ideal-pancake/
├── platformio.ini          # PlatformIO environment configuration
├── src/
│   └── main.cpp            # Your firmware entry point (setup + loop)
├── include/                # Put shared header (.h) files here
├── lib/                    # Put private libraries here
└── scripts/
    └── auto_build_upload.py  # Automated build + upload helper (Windows-friendly)
```

- **`platformio.ini`** — tells PlatformIO which board (`esp32-s3-devkitc-1`), framework (`arduino`), and serial speed to use.
- **`src/main.cpp`** — the main firmware file. `setup()` runs once at startup; `loop()` runs continuously.

---

### Step 4 — Write Your Firmware

Open `src/main.cpp` in any text editor or IDE. The starter code looks like this:

```cpp
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-S3 ready.");
}

void loop() {
  // put your main code here, to run repeatedly:
}
```

Add your own logic inside `setup()` (for one-time initialization) and `loop()` (for code that repeats forever). For example, to blink the built-in LED:

```cpp
#include <Arduino.h>

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("ESP32-S3 ready.");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```

---

### Step 5 — Build the Firmware

In the project root, run:

```bash
pio run -e esp32s3
```

PlatformIO will download the required toolchain and libraries on the first run (this may take a few minutes). A successful build ends with a message like:

```
SUCCESS
```

The compiled binary is written to `.pio/build/esp32s3/firmware.bin`.

---

### Step 6 — Connect the Board

1. Plug your ESP32-S3 board into a USB port.
2. Windows will assign it a COM port (e.g., `COM3`). You can find it in *Device Manager → Ports (COM & LPT)*.
3. On Linux the port appears as `/dev/ttyUSB0` or `/dev/ttyACM0`; on macOS it appears as `/dev/cu.usbserial-*` or `/dev/cu.wchusbserial*`.

---

### Step 7 — Upload the Firmware

**Option A — PlatformIO (recommended):**

```bash
pio run -e esp32s3 -t upload
```

PlatformIO auto-detects the port and flashes the board.

**Option B — Automated helper script (Windows-friendly, requires Step 1 optional packages):**

```bash
python scripts/auto_build_upload.py
```

The script:
1. Tries `pio run -e esp32s3 -t upload` first.
2. If that fails, it builds the firmware, auto-detects the COM port (prefers `COM3` and ports matching ESP/CH340/CP210 descriptions), and calls `esptool.py` to flash.

**Option C — Manual `esptool.py`** (if you know the COM port, e.g., `COM3`):

```bash
esptool.py --chip esp32s3 -p COM3 -b 460800 write_flash -z 0x0 .pio/build/esp32s3/firmware.bin
```

---

### Step 8 — Monitor Serial Output

After uploading, open the serial monitor to see debug messages printed with `Serial.println()`:

```bash
pio device monitor -e esp32s3
```

The monitor uses **115200 baud** (set in `platformio.ini`). Press **Ctrl+C** to exit.

---

### Step 9 — Iterate

Edit `src/main.cpp` → save → repeat Steps 5–8 to build, upload, and verify your changes.

---

## Quick Reference

| Task | Command |
|---|---|
| Build | `pio run -e esp32s3` |
| Build + Upload | `pio run -e esp32s3 -t upload` |
| Serial Monitor | `pio device monitor -e esp32s3` |
| Automated upload | `python scripts/auto_build_upload.py` |
| Clean build files | `pio run -e esp32s3 -t clean` |