#include <key.h>
#include <WiFiUser.h>
#include <EEPROM.h>
#include <test.h>


uint8_t	KeyDownFlag=0;

uint8_t	KeyFilterIn[TOTAL_KEY];
uint8_t	KeyFilterOut[TOTAL_KEY];

uint8_t KeyRepeatStrikeEn[TOTAL_KEY];
uint16_t KeyRepaetStrikeTimer[TOTAL_KEY];
uint8_t	KeyLongTimerFlg[TOTAL_KEY];

const uint8_t KeyMessageTable[TOTAL_KEY] = {KEY_1_DW};

void KeyScan() 
{ 				
	uint8_t PreKeyFilterOut;
	uint8_t KeyState[TOTAL_KEY];
	uint8_t KeyMessage;
	int i;
	
	KeyState[0] = (uint8_t)digitalRead(RESET_PIN);
	
	for(i=0; i<TOTAL_KEY; i++) {
		KeyFilterIn[i] <<= 1;
		KeyFilterIn[i] |= KeyState[i];
		if(	(KeyFilterIn[i] == 0xff) || (KeyFilterIn[i] == 0) ) {
			PreKeyFilterOut = KeyFilterOut[i];
			KeyFilterOut[i] = KeyFilterIn[i];
			
			if(KeyFilterOut[i] ^ PreKeyFilterOut) {
				KeyMessage = KeyMessageTable[i];
				
				if(KeyFilterOut[i]) {	 
					KeyMessage |= 0x80;
					KeyRepeatStrikeEn[i] = 0;
					KeyLongTimerFlg[i] = 0; 
				}
				else {
					KeyRepeatStrikeEn[i] = 1;
					KeyLongTimerFlg[i] = 1; 
					KeyRepaetStrikeTimer[i] = KEY_REP_STRIKE_TC;				
				}
                KeyMessageProc(KeyMessage);				
			}
		}
	}
	KeyRepeatStrikeScan();
}

void InitKeyInterface()
{
	int	i;
	uint8_t State[TOTAL_KEY];

	pinMode(RESET_PIN, INPUT);     //按键输入模式(默认高电平输入,按下时接到低电平)
	delay(200);
	State[0] = (uint8_t)digitalRead(RESET_PIN);
	
	for(i=0; i<TOTAL_KEY; i++) {

		if(State[i]) {
			KeyFilterIn[i] = 0xff;
			KeyFilterOut[i] = 0xff;
			KeyRepeatStrikeEn[i] = 0;
			KeyRepaetStrikeTimer[i] = 0;
			KeyLongTimerFlg[i] = 0; 
		}
		else {
			KeyFilterIn[i] = 0;
			KeyFilterOut[i] = 0;	
			KeyRepeatStrikeEn[i] = 0;
			KeyRepaetStrikeTimer[i] = 0;
			KeyLongTimerFlg[i] = 0; 			
		}
	}
}

void KeyRepeatStrikeScan()
{
	int	i;

	for(i=0; i<TOTAL_KEY; i++) {

		if(KeyLongTimerFlg[i]) {

			if(KeyRepeatStrikeEn[i]) {

				KeyRepaetStrikeTimer[i]--;
				
				if(KeyRepaetStrikeTimer[i] == 0) {

					KeyMessageLongProc(KeyMessageTable[i]);
					KeyLongTimerFlg[i]=0;	//屏蔽连击功能
				}
			}	
		}
	}
}

int KeyMessageProc(uint8_t KeyMessage)
{
	int Result = 1;

	switch(KeyMessage)
	{
		case KEY_1_DW:
				KeyDownFlag = 1;  //按键已按下
			break;

		case KEY_1_UP:
				KeyDownFlag = 0;  //按键已松开

				test_key_timer_cnt = 0;
				test_key_cnt++;  //1秒内短按3次，定时器自动清零

				if(test_key_cnt>=3){   //短按3下，进入测试模式
				    test_key_cnt = 0;
					test_mode_flag = 1;
					Serial.println("\n进入测试模式");
				}				
			break;
		
		default: 
			Result = 0;
			break;
	}
	return Result;
}

int KeyMessageLongProc(uint8_t KeyMessage)
{
	int Result = 1;

	switch(KeyMessage)
	{
	    case KEY_1_DW:
                Serial.println("\n按键已长按4秒,正在清空网络连保存接信息.");
                deletewificonfig(); //删除EEROM保存的wifi信息
                restoreWiFi();     //删除保存的wifi信息
                ESP.restart();     //重启复位esp32	
			break;
		default: 
			Result = 0;
			break;
	}
	return Result;
}
