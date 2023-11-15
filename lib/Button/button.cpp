#include "button.h"

#define ABS(n)        ((n) > 0 ? (n) : -(n))
#define COUNT(n)      (sizeof(n)/sizeof(n[0]))

void Button_Init(uint8_t pin)
{
  pinMode(pin, INPUT_PULLUP);
}

typedef struct {
  uint32_t ms_stamp;
  uint32_t nums_false;
  uint32_t nums_true;
  uint8_t  value;
} btn_pri_t;

// read hardware button
uint8_t Btn_IsPressed(btn_pri_t *pButton, uint8_t pin)
{
  uint32_t now = millis();
  uint32_t next = pButton->ms_stamp;
//  if(OS_GetTimeMs() > pButton->ms_stamp)
  if(now > next)
  {
    pButton->value = (pButton->nums_true > pButton->nums_false) ? true : false;
    pButton->nums_true = 0;
    pButton->nums_false = 0;
    pButton->ms_stamp = millis() + BTN_FILTER_MS;
  }
  else
  {
    if(digitalRead(pin) == 0)
    {
      pButton->nums_true++;
    }
    else
    {
      pButton->nums_false++;
    }
  }

  return pButton->value;
}

typedef struct {
  uint8_t    pressed;
  uint32_t   ms_stamp;
  v_btn_t    action;
  uint16_t   long_press_cycle;
  ACT_STATUS status;
  uint8_t config_en;
} btn_action_t;

v_btn_t Btn_GetAction(btn_action_t *pAction)
{
  v_btn_t btn_v = BTN_IDLE;
  uint32_t now_ms = millis();

  if (pAction->pressed)
  {
    switch (pAction->status)
    {
      case NO_CLICK:
        pAction->status = FIRST_CLICK;
        pAction->ms_stamp = now_ms;
        pAction->action = BTN_CLICK;
        break;

#ifdef ENABLE_LONG_PRESS
      case FIRST_CLICK:
      case SECOND_CLICK:
      case TRIPLE_CLICK:
        if (pAction->config_en & EN_LONG_PRESS)
        {
          if (now_ms - pAction->ms_stamp > BTN_LONG_PRESS_MS)
          {
            pAction->status = LONG_PRESS;
            pAction->action = BTN_LONG_CLICK;
          }
        }
        break;
#endif

#ifdef ENABLE_DOUBLE
      case FIRST_RELEASE:
        if (pAction->config_en & EN_DOUBLE)
        {
          pAction->status = SECOND_CLICK;
          pAction->action = BTN_DOUBLE_CLICK;
        }
        break;
#endif

#ifdef ENABLE_TRIPLE
      case SECOND_RELEASE:
        if (pAction->config_en & EN_TRIPLE)
        {
          pAction->status = TRIPLE_CLICK;
          pAction->action = BTN_TRIPLE_CLICK;
        }
        break;
#endif

#ifdef ENABLE_LONG_PRESS

  #ifdef ENABLE_LONG_PRESS_HOLD
      case LONG_PRESS:
        if (pAction->config_en & EN_LONG_PRESS_HOLD)
        {
          if (now_ms - pAction->ms_stamp > pAction->long_press_cycle)
          {
            if (pAction->long_press_cycle > BTN_LONG_PRESS_MS_MIN)
              pAction->long_press_cycle -= BTN_LONG_PRESS_MS_SUB;
            else
              pAction->long_press_cycle = BTN_LONG_PRESS_MS_MIN;
            pAction->ms_stamp = now_ms;
            btn_v = pAction->action;
          }
        }
        break;
  #endif
#endif
      default:
        break;
    }
  }
  else
  {
    switch (pAction->status)
    {
      case FIRST_CLICK:
        pAction->status = FIRST_RELEASE;
        break;

      case FIRST_RELEASE:
#ifdef ENABLE_DOUBLE
        if (((pAction->config_en & EN_DOUBLE) == 0) || (now_ms - pAction->ms_stamp > BTN_DOUOBLE_MS))
#endif
        {
          pAction->status = NO_CLICK;
          btn_v = pAction->action;
        }
        break;

#ifdef ENABLE_DOUBLE
      case SECOND_CLICK:
        pAction->status = SECOND_RELEASE;
        break;

      case SECOND_RELEASE:
#ifdef ENABLE_DOUBLE
        if (((pAction->config_en & EN_TRIPLE) == 0) || (now_ms - pAction->ms_stamp > BTN_TRIPLE_MS))
#endif
        {
          pAction->status = NO_CLICK;
          btn_v = pAction->action;
        }
        break;
#endif

#ifdef ENABLE_TRIPLE
      case TRIPLE_CLICK:
        if (pAction->config_en & EN_TRIPLE)
        {
          btn_v = pAction->action;
          pAction->status = NO_CLICK;
        }
        break;
#endif

#ifdef ENABLE_LONG_PRESS
      case LONG_PRESS:
        if (pAction->config_en & EN_LONG_PRESS)
        {
          pAction->status = NO_CLICK;
          pAction->long_press_cycle = BTN_LONG_PRESS_MS;
          btn_v = BTN_LONG_RELEASE;
        }
        break;
#endif

      default:
        break;
    }
  }
  return btn_v;
}



v_btn_t Button_GetAction(uint8_t pin)
{
  static btn_pri_t isPressed = {  
    .ms_stamp = 0,
    .nums_false = 0,
    .nums_true = 0,
    .value = 0,
  };
  static btn_action_t btnAction = {  
    .pressed = 0,
    .ms_stamp = 0,
    .action = 0,
#ifdef ENABLE_LONG_PRESS_HOLD
    .long_press_cycle = BTN_LONG_PRESS_MS_MAX,
#endif
    .status = NO_CLICK,
    .config_en = EN_DOUBLE | EN_TRIPLE | EN_LONG_PRESS | EN_LONG_PRESS_HOLD,
  };

  btnAction.pressed = Btn_IsPressed(&isPressed, pin);

  return Btn_GetAction(&btnAction);
}
