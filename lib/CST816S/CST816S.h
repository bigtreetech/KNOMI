#ifndef CST816S_H
#define CST816S_H

#include <Arduino.h>
#include <Wire.h>

/** I2C ADDRESS/BITS **/
#define CST816S_DEFAULT_ADDRESS (0x15)

/**
 * 0x00: no gesture 
 * 0x01: swipe up
 * 0x02: swipe down
 * 0x03: swipe left
 * 0x04: swipe right
 * 0x05: single click
 * 0x0B: double click
 * 0x0C: long press
 */
#define CST816S_REG_GESTURE_ID  0x01
#define CST816S_REG_FINGER_NUM  0x02
/**
 * 0x00: first touch 
 * 0x08: touching
 * 0x04: released
 */
#define CST816S_REG_STATUS      0x03
#define CST816S_REG_XPOS_H4     0x03
#define CST816S_REG_XPOS_L8     0x04
#define CST816S_REG_YPOS_H4     0x05
#define CST816S_REG_YPOS_L8     0x06
/**
 *  CST716 : 0xA5
 * CST816D : 0xE5
 * CST816S : 0xE5
 * CST816T : 0xE5
 * write value 0x03 to sleep
 */
#define CST816S_REG_SLEEP       0xE5
/**
 *  CST716 : 0x20
 * CST816S : 0xB4
 * CST816T : 0xB5
 * CST816D : 0xB6
 */
#define CST816S_REG_DEVICE_ID     0xA7
#define CST816S_REG_PROJECT_ID    0xA8
#define CST816S_REG_FIRMWARE_VER  0xA9
/* Enable MotionMask */
#define CST816S_REG_MOTION_MASK   0xEC
/* Report interrupt cycle, unit: 10ms */
#define CST816S_REG_REPORT_RATE   0xEE
/* Report interrupt mode
 * Bit7: EnTest, Interrupt test, auto send low pulses periodically after enabled
 * Bit6: EnTouch, send low pulses periodically when detected touch
 * Bit5: EnChange,  send low pulses when detected touch status changed
 * Bit4: EnMotion, send low pulses when detected gesture
 * Bit0: OnceWLP, only one pulse for long press
 */
#define CST816S_REG_REPORT_MODE   0xFA
/* Auto reset when touched but no valid gesture, unit: 1s */
#define CST816S_REG_AUTO_RST_SEC  0xFB
/* Auto reset when long time touched, unit: 1s */
#define CST816S_REG_LONG_RST_SEC  0xFC
/* 
 * 0: enable auto sleep
 * else: disable auto sleep
 */
#define CST816S_REG_DISABLE_AUTO_SLEEP 0xFE

enum CST_GESTURE {
  GESTURE_NONE = 0x00,
  GESTURE_SCROLL_UP = 0x01,
  GESTURE_SCROLL_DOWN = 0x02,
  GESTURE_SCROLL_LEFT = 0x03,
  GESTURE_SCROLL_RIGHT = 0x04,
  GESTURE_CLICK = 0x05,
  GESTURE_DOUBLE_CLICK = 0x0B,
  GESTURE_LONG_PRESS = 0x0C
};

enum CST_STATUS {
  STATUS_FIRST_PRESS = 0x00,
  STATUS_PRESSED = 0x08,
  STATUS_RELEASED = 0x04,
};

typedef struct {
  uint8_t gesture; // CST_GESTURE
  uint8_t finger;
  uint8_t status; // CST_STATUS
  uint16_t x;
  uint16_t y;
} touch_event_t;

class CST816S {
public:
  CST816S(int rst, int irq, TwoWire *Wi = &Wire);

  bool begin(uint8_t addr = CST816S_DEFAULT_ADDRESS, int interrupt = FALLING, uint8_t id = 0xB4);
  
  bool ready(void);
  void wakeup(void);
  uint8_t getDeviceID(void);
  uint8_t getFirmwareVer(void);

  bool setMotionMask(uint8_t mask);
  bool setReportRate(uint8_t _10ms);
  bool setReportMode(uint8_t mode);
  bool setAutoRst(uint8_t sec);
  bool setLongRst(uint8_t sec);
  bool setDisAutoSleep(uint8_t dissleep);
  bool getTouch(touch_event_t *event);

private:
  int _rst;
  int _irq;
  TwoWire *i2c;
  int8_t _i2caddr;

  bool _ready;
  void _setReady(void);
  void _reset(void);
  bool read_raw(uint8_t reg, uint8_t *data, uint32_t len);
  bool write_raw(uint8_t reg, uint8_t *data, uint32_t len);
};

#endif
