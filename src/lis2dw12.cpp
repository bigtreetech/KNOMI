#include <Arduino.h>
#include <LIS2DW12Sensor.h>
#include "pinout.h"

#ifdef LIS2DW_SUPPORT

// #define LIS2DW12_DEBUG

extern TwoWire i2c0;
LIS2DW12Sensor lis2dw12 = LIS2DW12Sensor(&i2c0, LIS2DW12_I2C_ADD_H);

extern LIS2DW12Sensor lis2dw12;
int32_t lis2dw12_acc[3];

//
void lis2dw12_task(void * parameter) {
    Serial.println("\r\n******** LIS2DW12 startup *****\r\n");
    if (lis2dw12.begin()) {   // change this to 0x19 for alternative i2c address
        while (1) {
            Serial.println("Couldn't find lis2dw12");
            delay(1000);
        }
    }
    Serial.println("LIS2DW12 found!");
    lis2dw12.Enable_X();
    Serial.println("\r\n******** LIS2DW12 init ok *****\r\n");

    for(;;) {
        lis2dw12.Get_X_Axes(lis2dw12_acc); // get X Y and Z data at once
        // Due to installation, y and z need to be exchanged
        int32_t tmp = lis2dw12_acc[1];
        lis2dw12_acc[1] = lis2dw12_acc[2];
        lis2dw12_acc[2] = tmp;
#ifdef LIS2DW12_DEBUG
        // Then print out the raw data
        Serial.println("\r\n******** lis2dw12 data *****\r\n");
        Serial.print("X: "); Serial.println(lis2dw12_acc[0]);
        Serial.print("Y: "); Serial.println(lis2dw12_acc[1]);
        Serial.print("Z: "); Serial.println(lis2dw12_acc[2]);
        Serial.println("\r\n****************************\r\n");
#endif
        delay(100);
    }
}

#endif
