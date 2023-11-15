#ifndef __KEY_H__
#define __KEY_H__

#include <Arduino.h>

#define RESET_PIN           0    //设置重置按键引脚,用于删除WiFi信息

#define TOTAL_KEY			1    //1个按键
#define KEY_SCAN_INTERVAL	4    //扫描时间4*8=32ms滤波时间
#define KEY_REP_STRIKE_TC   4000  //长按时间4*800大约4秒

#define KEY_1_DW		  0x01
#define KEY_1_UP		  0x81

#define KEY_DWON		  1   //按键接通

extern uint8_t	KeyDownFlag;   //按键按下标志位，避免其它任务占用过长时间影响按键判断

void KeyScan();
void InitKeyInterface();
int KeyMessageProc(uint8_t KeyMessage);
int KeyMessageLongProc(uint8_t KeyMessage);
void KeyRepeatStrikeScan();

void cls();
#endif