#ifndef PINOUT_KNOMI_V2_H
#define PINOUT_KNOMI_V2_H

#define BOOT_PIN     0

// common i2c
#define I2C0_SUPPORT
#define I2C0_SPEED   100000
#define I2C0_SCL_PIN 1
#define I2C0_SDA_PIN 2
// #define I2C1_SPEED   100000
// #define I2C1_SCL_PIN 3
// #define I2C1_SDA_PIN 4

// GC9A01 SPI TFT
// #define GC9A01_MISO_PIN 13
#define GC9A01_MOSI_PIN 14
#define GC9A01_SCLK_PIN 18
#define GC9A01_CS_PIN   20
#define GC9A01_DC_PIN   19
#define GC9A01_RST_PIN  21
// PWM
#define LCD_BL_PIN         12

// CST816S Touch
#define CST816S_SUPPORT
#define CST816S_IRQ_PIN 17
#define CST816S_RST_PIN 16

// LIS2DW Accelerometer
#define LIS2DW_SUPPORT

// OV2640 Camera
#define CAM_PWDN_PIN  10
#define CAM_RESET_PIN 11
#define CAM_XCLK_PIN  7
#define CAM_SDA_PIN   I2C1_SDA_PIN
#define CAM_SCL_PIN   I2C1_SCL_PIN
#define CAM_D9_PIN    8
#define CAM_D8_PIN    6
#define CAM_D7_PIN    5
#define CAM_D6_PIN    47
#define CAM_D5_PIN    39
#define CAM_D4_PIN    41
#define CAM_D3_PIN    40
#define CAM_D2_PIN    38
#define CAM_D1_PIN    42
#define CAM_D0_PIN    45
#define CAM_VSYNC_PIN 15
#define CAM_HREF_PIN  9
#define CAM_PCLK_PIN  48

// Button
#define BTN_1_PIN I2C1_SDA_PIN
#define BTN_2_PIN I2C1_SCL_PIN
#define BTN_3_PIN CAM_RESET_PIN
// LED
#define LED_2_PIN CAM_D0_PIN
#define LED_3_PIN CAM_D1_PIN
#define LED_4_PIN CAM_D4_PIN

#endif
