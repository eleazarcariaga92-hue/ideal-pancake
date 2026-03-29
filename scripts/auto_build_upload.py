#!/usr/bin/env python3
"""
Auto build + upload helper for PlatformIO on Windows.

Behavior:
1) Try: `pio run -e esp32s3 -t upload` (PlatformIO upload, auto-detect COM).
2) If that fails, run a build and locate .pio/build/esp32s3/firmware.bin
   then auto-detect a COM port (pyserial) and call esptool.py to flash it.

Usage:
  1) Install prerequisites: pip install pyserial esptool
  2) Run from project root: python scripts/auto_build_upload.py
"""
from __future__ import print_function
import os
import sys
import subprocess
import time

try:
    from serial.tools import list_ports
except Exception:
    print("Missing pyserial. Install with: pip install pyserial")
    sys.exit(1)

PROJECT_ENV = "esp32s3"
BUILD_DIR = os.path.join(".pio", "build", PROJECT_ENV)
FIRMWARE_BIN = os.path.join(BUILD_DIR, "firmware.bin")

def run(cmd, check=False, capture=False):
    print("> " + " ".join(cmd))
    if capture:
        return subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    return subprocess.run(cmd, check=check)

def try_pio_upload():
    print("Attempting PlatformIO upload (auto-detect COM)...")
    res = subprocess.run(["pio", "run", "-e", PROJECT_ENV, "-t", "upload"])
    return res.returncode == 0

def ensure_build():
    print("Building firmware with PlatformIO...")
    res = subprocess.run(["pio", "run", "-e", PROJECT_ENV])
    return res.returncode == 0

def find_com_port(preferred_list=None):
    # preferred_list: optional list like ["COM3"]
    if preferred_list:
        for p in preferred_list:
            # check if port exists
            ports = [cp.device for cp in list_ports.comports()]
            if p in ports:
                return p

    ports = list(list_ports.comports())
    if not ports:
        return None

    # Heuristics: prefer ports whose description mentions ESP, CDC, JTAG, USB, Silicon Labs, CH910, CP210
    keywords = ["ESP", "CDC", "USB", "Silicon", "CP210", "CH910", "CH340", "FT232"]
    scored = []
    for p in ports:
        score = 0
        desc = (p.description or "") + " " + (p.manufacturer or "") + " " + (p.hwid or "")
        for kw in keywords:
            if kw.lower() in desc.lower():
                score += 10
        # prefer COM numbers >= 3 (avoid system COM1/COM2)
        try:
            num = int(p.device.replace("COM", ""))
            if num >= 3:
                score += 1
        except Exception:
            pass
        scored.append((score, p.device, desc))
    scored.sort(reverse=True)
    # choose highest scoring
    return scored[0][1]

def flash_with_esptool(port, firmware):
    print(f"Flashing {firmware} to {port} with esptool.py (esp32s3)...")
    cmd = [
        "esptool.py",
        "--chip", "esp32s3",
        "-p", port,
        "-b", "460800",
        "write_flash",
        "-z",
        "0x0",
        firmware
    ]
    return subprocess.run(cmd).returncode == 0

def main():
    # 1) Try PlatformIO upload (auto detect)
    if try_pio_upload():
        print("Upload via PlatformIO succeeded.")
        return 0
    print("PlatformIO upload failed or couldn't auto-detect. Falling back to esptool flow.")

    # 2) Ensure build exists
    if not os.path.exists(FIRMWARE_BIN):
        ok = ensure_build()
        if not ok:
            print("Build failed. See PlatformIO output.")
            return 2
        # small delay to let filesystem settle
        time.sleep(0.3)

    if not os.path.exists(FIRMWARE_BIN):
        print("Cannot find firmware at:", FIRMWARE_BIN)
        return 3

    # 3) Auto-detect COM port
    # If user previously used COM3 (common in your setup), try that first
    preferred_ports = ["COM3"]
    port = find_com_port(preferred_ports)
    if not port:
        print("No serial ports found. Please plug the board in and try again.")
        return 4

    print("Detected port:", port)

    # 4) Flash with esptool
    success = flash_with_esptool(port, FIRMWARE_BIN)
    if success:
        print("Flashing complete.")
        return 0
    print("esptool flashing failed. You can retry manually with:")
    print(f"  esptool.py --chip esp32s3 -p {port} -b 460800 write_flash -z 0x0 {FIRMWARE_BIN}")
    return 5

if __name__ == "__main__":
    sys.exit(main())
