#include "Arduino.h"

#include <CST816S.h>
#include <Wire.h>
#include <FunctionalInterrupt.h>

/*!
 *  @brief  Constructor for CST816S
 *  @param  rst
 *          touch reset pin
 *  @param  irq
 *          touch interrupt pin
 *  @param  Wi
 *          optional wire object
 */
CST816S::CST816S(int rst, int irq, TwoWire *Wi) {
  _rst = rst;
  _irq = irq;
  i2c = Wi;
}

/*!
 *  @brief  touch inturrupt
 */
void CST816S::_setReady(void) {
  _ready = true;
}

/*!
 *  @brief  reset touch screen
 */
void CST816S::_reset(void) {
  digitalWrite(_rst, LOW);
  delay(10);
  digitalWrite(_rst, HIGH );
  delay(50);
}

/*!
 *  @brief  wake up by reset
 */
void CST816S::wakeup(void) {
  _reset();
}

/*!
 *  @brief  Setups the HW
 *  @param  addr
 *          i2c address (optional, fallback to default)
 *  @param  interrupt
 *          type of interrupt(FALLING, RISING)
 *  @param  id
 *          device id register value - defaults to 0xB4 (CST816S)
 *  @return true if successful
 */
bool CST816S::begin(uint8_t addr, int interrupt, uint8_t id) {
  _i2caddr = addr;
  i2c->begin();
  pinMode(_irq, INPUT);
  pinMode(_rst, OUTPUT);
  _reset();
  
  attachInterrupt(_irq, std::bind(&CST816S::_setReady, this), interrupt);
    /* Check connection */
  // if (getDeviceID() != id) {
  //   /* No CST816S detected ... return false */
  //   // Serial.println(deviceid, HEX);
  //   return false;
  // }
  return true;
}

/*!
 *  @brief  is there a touch
 */
bool CST816S::ready(void) {
  if (_ready) {
    _ready = false;
    return true;
  }
  return false;
}

/*!
 *  @brief  Read raw register value from CST816S
 *  @param  reg
 *          register address
 *  @param  data
 *          where the read values are stored
 *  @param  len
 *          data length
 *  @return true if successful
 */
bool CST816S::read_raw(uint8_t reg, uint8_t *data, uint32_t len) {
  // Write message to the slave
  i2c->beginTransmission(_i2caddr);
  i2c->write(reg);
  if (i2c->endTransmission(true))
    return false;
  // Read bytes from the slave
  uint8_t bytesReceived = i2c->requestFrom(_i2caddr, len);
  if((bool)bytesReceived){ //If received more than zero bytes
    uint8_t temp[bytesReceived];
    i2c->readBytes(data, bytesReceived);
  }
  return true;
}

/*!
 *  @brief  Write raw register value to CST816S
 *  @param  reg
 *          register address
 *  @param  data
 *          where the register values from
 *  @param  len
 *          data length
 *  @return true if successful
 */
bool CST816S::write_raw(uint8_t reg, uint8_t *data, uint32_t len) {
  // Write message to the slave
  i2c->beginTransmission(_i2caddr);
  i2c->write(reg);  
  for (int i = 0; i < len; i++) {
    i2c->write(data[i]);
  }
  if (i2c->endTransmission(true))
    return false;
  return true;
}

/*!
 *  @brief  Get Device ID
 *  @return Device ID
 *             CST716 : 0x20
 *            CST816S : 0xB4
 *            CST816T : 0xB5
 *            CST816D : 0xB6
 */
uint8_t CST816S::getDeviceID(void) {
  uint8_t id;
  if (read_raw(CST816S_REG_DEVICE_ID, &id, 1)) {
    return id;
  }
  return 0xFF;
}

/*!
 *  @brief  Get firmware version
 */
uint8_t CST816S::getFirmwareVer(void) {
  uint8_t ver;
  if (read_raw(CST816S_REG_FIRMWARE_VER, &ver, 1)) {
    return ver;
  }
  return 0xFF;
}

/*!
 *  @brief  Set Motion Mask
 *  @param  mask:
 *            bit2: EnConLR, Enable continuous scroll left/right
 *            bit1: EnConUD, Enable continuous scroll up/down
 *            bit0: EnDClick, Enable double click
 *  @return true if successful
 */
bool CST816S::setMotionMask(uint8_t mask) {
  return write_raw(CST816S_REG_MOTION_MASK, &mask, 1);
}

/*!
 *  @brief  Set report interrupt cycle, unit: 10ms
 *  @param  _10ms
 *          cycle in 10ms
 *  @return true if successful
 */
bool CST816S::setReportRate(uint8_t _10ms) {
  return write_raw(CST816S_REG_REPORT_RATE, &_10ms, 1);
}

/*!
 *  @brief  Set report interrupt mode
 *  @param  mode
 *          0x60: touch generated interrupt
 *          0x11: gesture generated interrupt
 *          0x71: 0x60 + 0x11: touch + gesture
 *  @return true if successful
 */
bool CST816S::setReportMode(uint8_t mode) {
  return write_raw(CST816S_REG_REPORT_MODE, &mode, 1);
}

/*!
 *  @brief  Set auto reset when touched but no valid gesture
 *  @param  sec
 *          cycle in second           
 *  @return true if successful
 */
bool CST816S::setAutoRst(uint8_t sec) {
  return write_raw(CST816S_REG_AUTO_RST_SEC, &sec, 1);
}

/*!
 *  @brief  Set auto reset when long time touched
 *  @param  sec
 *          cycle in second           
 *  @return true if successful
 */
bool CST816S::setLongRst(uint8_t sec) {
  return write_raw(CST816S_REG_LONG_RST_SEC, &sec, 1);
}

/*!
 *  @brief  Set auto sleep
 *  @param  dissleep
 *          0: enable auto sleep
 *       else: disable auto sleep        
 *  @return true if successful
 */
bool CST816S::setDisAutoSleep(uint8_t dissleep) {
  return write_raw(CST816S_REG_DISABLE_AUTO_SLEEP, &dissleep, 1);
}

/*!
 *  @brief  Get touch info (gesture, finger, status, x&y postion)
 *  @param  event
 *          where the info are stored
 *  @return true if successful
 */
bool CST816S::getTouch(touch_event_t *event){
  uint8_t raw[6];
  if (read_raw(CST816S_REG_GESTURE_ID, raw, 6)) {
    event->gesture = raw[0];
    event->finger = raw[1];
    event->status = (raw[2] >> 4) & 0x0F;
    event->x = ((raw[2] & 0x0F) << 8) | raw[3];
    event->y = ((raw[4] & 0x0F) << 8) | raw[5];
    return true;
  }
  return false;
}
