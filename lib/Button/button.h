#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <Arduino.h>
#include <Wire.h>

// Button

typedef uint16_t v_btn_t;

#define ENABLE_DOUBLE
#define ENABLE_TRIPLE
#define ENABLE_LONG_PRESS
#define ENABLE_LONG_PRESS_HOLD

typedef enum
{
  EN_DOUBLE = (1 << 0),
  EN_TRIPLE = (1 << 1),
  EN_LONG_PRESS = (1 << 2),
  EN_LONG_PRESS_HOLD = (1 << 3),
} CONFIG_FUN;


#define BTN_IDLE         0x0
#define BTN_CLICK        0x0100
#define BTN_DOUBLE_CLICK 0x0200
#define BTN_TRIPLE_CLICK 0x0400
#define BTN_LONG_CLICK   0x0800
#define BTN_LONG_RELEASE 0x1000

typedef enum
{
  NO_CLICK = 0,
  FIRST_CLICK,
  FIRST_RELEASE,
  SECOND_CLICK,
  SECOND_RELEASE,
  TRIPLE_CLICK,
  LONG_PRESS,
} ACT_STATUS;

#define BTN_FILTER_MS         50
#define BTN_DOUOBLE_MS        500
#define BTN_TRIPLE_MS         1000
#define BTN_LONG_PRESS_MS     5000
#ifdef ENABLE_LONG_PRESS_HOLD
  #define BTN_LONG_PRESS_MS_MAX BTN_LONG_PRESS_MS
  #define BTN_LONG_PRESS_MS_MIN 20
  #define BTN_LONG_PRESS_MS_SUB 990
#endif

void Button_Init(uint8_t pin);
v_btn_t Button_GetAction(uint8_t pin);

#endif
