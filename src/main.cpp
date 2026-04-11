/**
 * Waveshare ESP32-S3-Touch-LCD-3.5  –  auto-configured firmware
 *
 * Hardware:
 *   Display  : ST7796 (SPI, 320×480 portrait / 480×320 landscape)
 *   Touch    : FT6336 capacitive (I²C address 0x38)
 *   PMIC     : AXP2101 (I²C address 0x34) — controls the LCD backlight via ALDO2
 *
 * Pin map (from official Waveshare schematic):
 *   SPI display  MOSI=10  SCLK=11  MISO=12  CS=13  DC=15  RST=17
 *   I²C (touch + PMIC)     SDA=8   SCL=7
 *   Touch INT=3  Touch RST=2
 *
 * What this sketch does:
 *   1. Initialises the AXP2101 PMIC to power the backlight / peripherals.
 *   2. Resets and initialises the FT6336 touch controller.
 *   3. Draws a splash screen via TFT_eSPI.
 *   4. Runs a finger-paint loop — touch the screen to draw coloured dots.
 */

#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>  // configured entirely through platformio.ini build_flags

// ─── I²C bus pins ─────────────────────────────────────────────────────────────
#define I2C_SDA  8
#define I2C_SCL  7

// ─── Touch controller (FT6336) ────────────────────────────────────────────────
#define TOUCH_INT    3
#define TOUCH_RST    2
#define FT6336_ADDR  0x38

// ─── AXP2101 PMIC ─────────────────────────────────────────────────────────────
#define AXP2101_ADDR  0x34

// AXP2101 register map (relevant subset)
#define AXP_DCDC1_V   0x82   // DC-DC1 output voltage (step 100 mV, base 1500 mV)
#define AXP_LDO_EN    0x90   // LDO enable bitmap
                             //   bit0=ALDO1  bit1=ALDO2  bit2=ALDO3  bit3=ALDO4
                             //   bit4=BLDO1  bit5=BLDO2
#define AXP_ALDO1_V   0x92   // ALDO1 voltage (step 100 mV, base 500 mV)
#define AXP_ALDO2_V   0x93   // ALDO2 voltage  ← LCD backlight power on this board
#define AXP_ALDO3_V   0x94   // ALDO3 voltage  ← touch + peripheral VCC
#define AXP_BLDO1_V   0x96   // BLDO1 voltage

// ─── Display object ───────────────────────────────────────────────────────────
TFT_eSPI tft;

// ─────────────────────────────────────────────────────────────────────────────
// I²C helpers
// ─────────────────────────────────────────────────────────────────────────────

static bool i2c_write(uint8_t dev, uint8_t reg, uint8_t val) {
    Wire.beginTransmission(dev);
    Wire.write(reg);
    Wire.write(val);
    return Wire.endTransmission() == 0;
}

static uint8_t i2c_read(uint8_t dev, uint8_t reg) {
    Wire.beginTransmission(dev);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return 0xFF;
    if (Wire.requestFrom(dev, (uint8_t)1) < 1) return 0xFF;
    return Wire.read();
}

static bool i2c_ping(uint8_t dev) {
    Wire.beginTransmission(dev);
    return Wire.endTransmission() == 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// AXP2101 – minimal init (enable backlight + peripheral LDOs)
// ─────────────────────────────────────────────────────────────────────────────

static void axp2101_init() {
    // DC-DC1 → 3.3 V  (core system rail; should already be on, but set explicitly)
    i2c_write(AXP2101_ADDR, AXP_DCDC1_V, (3300 - 1500) / 100);  // = 18

    // ALDO1 → 3.3 V  (general peripherals)
    i2c_write(AXP2101_ADDR, AXP_ALDO1_V, (3300 - 500) / 100);   // = 28
    // ALDO2 → 3.3 V  (LCD backlight power on the 3.5" board)
    i2c_write(AXP2101_ADDR, AXP_ALDO2_V, (3300 - 500) / 100);
    // ALDO3 → 3.3 V  (touch controller VCC)
    i2c_write(AXP2101_ADDR, AXP_ALDO3_V, (3300 - 500) / 100);
    // BLDO1 → 3.3 V  (additional safety; some revisions route BL here)
    i2c_write(AXP2101_ADDR, AXP_BLDO1_V, (3300 - 500) / 100);

    // Enable ALDO1 | ALDO2 | ALDO3 | BLDO1
    uint8_t en = i2c_read(AXP2101_ADDR, AXP_LDO_EN);
    if (en == 0xFF) en = 0;
    i2c_write(AXP2101_ADDR, AXP_LDO_EN, en | 0x17);  // bits 0,1,2,4

    Serial.println("[AXP2101] PMIC init done – backlight on");
}

// ─────────────────────────────────────────────────────────────────────────────
// FT6336 – read first touch point
// ─────────────────────────────────────────────────────────────────────────────

static bool touch_read(uint16_t &x, uint16_t &y) {
    Wire.beginTransmission(FT6336_ADDR);
    Wire.write(0x02);  // TD_STATUS (touch count)
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(FT6336_ADDR, (uint8_t)5) < 5) return false;

    uint8_t points = Wire.read() & 0x0F;
    if (points == 0 || points > 5) return false;

    uint8_t xh = Wire.read();   // event flag + x[11:8]
    uint8_t xl = Wire.read();   // x[7:0]
    uint8_t yh = Wire.read();   // touch id  + y[11:8]
    uint8_t yl = Wire.read();   // y[7:0]

    x = ((uint16_t)(xh & 0x0F) << 8) | xl;
    y = ((uint16_t)(yh & 0x0F) << 8) | yl;
    return true;
}

// ─── Screen dimensions in landscape orientation ───────────────────────────────
// TFT_WIDTH / TFT_HEIGHT are the portrait-native driver dimensions defined in
// platformio.ini.  After setRotation(1) the logical screen is W × H swapped.
static constexpr uint16_t SCREEN_W = TFT_HEIGHT;  // 480
static constexpr uint16_t SCREEN_H = TFT_WIDTH;   // 320

// Paint colour palette (cycles on each lift)
static const uint16_t PALETTE[] = {
    TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW,
    TFT_CYAN, TFT_MAGENTA, TFT_WHITE, TFT_ORANGE
};
static uint8_t colour_idx = 0;

// ─────────────────────────────────────────────────────────────────────────────
// setup
// ─────────────────────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Waveshare ESP32-S3-Touch-LCD-3.5 ===");

    // ── I²C bus (touch + PMIC share the same bus) ─────────────────────────────
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(400000);

    // ── Touch hardware reset ───────────────────────────────────────────────────
    pinMode(TOUCH_RST, OUTPUT);
    digitalWrite(TOUCH_RST, LOW);
    delay(10);
    digitalWrite(TOUCH_RST, HIGH);
    delay(50);

    // ── AXP2101 PMIC (backlight + peripheral power) ───────────────────────────
    if (i2c_ping(AXP2101_ADDR)) {
        axp2101_init();
    } else {
        Serial.println("[AXP2101] not found – backlight may need manual GPIO");
    }

    // ── Touch controller presence check ───────────────────────────────────────
    if (i2c_ping(FT6336_ADDR)) {
        Serial.println("[FT6336] touch controller found");
    } else {
        Serial.println("[FT6336] touch controller NOT found on bus");
    }

    // ── Display ───────────────────────────────────────────────────────────────
    tft.init();
    tft.setRotation(1);       // landscape: 480 × 320
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    // Splash screen
    tft.fillRect(0, 0, SCREEN_W, 50, TFT_NAVY);
    tft.setTextColor(TFT_WHITE, TFT_NAVY);
    tft.drawString("Waveshare ESP32-S3-Touch-LCD-3.5", SCREEN_W / 2, 25, 2);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("ST7796  480x320  FT6336 touch", SCREEN_W / 2, 90, 2);
    tft.drawString("AXP2101 PMIC backlight", SCREEN_W / 2, 115, 2);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Touch the screen to paint!", SCREEN_W / 2, 160, 2);

    // Colour swatches
    for (int i = 0; i < 8; i++) {
        tft.fillRect(20 + i * 56, 230, 50, 40, PALETTE[i]);
    }
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString("colour auto-cycles on each lift", SCREEN_W / 2, 285, 1);

    Serial.println("[TFT] display ready");
}

// ─────────────────────────────────────────────────────────────────────────────
// loop – finger-paint demo
// ─────────────────────────────────────────────────────────────────────────────

void loop() {
    static bool was_touched = false;

    uint16_t tx, ty;
    bool touched = touch_read(tx, ty);

    if (touched) {
        // The FT6336 reports in panel-native (portrait) coordinates.
        // Map to landscape screen (480 × 320):
        //   panel-x (0..319) → screen-y inverted
        //   panel-y (0..479) → screen-x
        uint16_t sx = ty;
        uint16_t sy = (SCREEN_H - 1) - tx;
        sx = min(sx, (uint16_t)(SCREEN_W - 1));
        sy = min(sy, (uint16_t)(SCREEN_H - 1));

        tft.fillCircle(sx, sy, 5, PALETTE[colour_idx]);
        Serial.printf("[touch] raw(%u,%u) -> screen(%u,%u)\n", tx, ty, sx, sy);
        was_touched = true;
    } else if (was_touched) {
        // Finger lifted – advance colour
        colour_idx = (colour_idx + 1) % 8;
        was_touched = false;
    }

    delay(8);
}
