#include <Arduino.h>
#include <Adafruit_SHT4x.h>

// #define SHT4X_DEBUG

typedef struct {
    float temp;
    float humi;
}sht4x_data_t;

extern TwoWire i2c0;
Adafruit_SHT4x sht4x = Adafruit_SHT4x();

sht4x_data_t sht4x_data;

//
void sht4x_task(void * parameter) {
    Serial.println("\r\n******** SHT4x startup *****\r\n");
    if (!sht4x.begin(&i2c0)) {
        while (1) {
            Serial.println("Couldn't find SHT4x");
            delay(1000);
        }
    }
    Serial.println("Found SHT4x sensor");
    Serial.print("Serial number 0x");
    Serial.println(sht4x.readSerial(), HEX);

    // You can have 3 different precisions, higher precision takes longer
    sht4x.setPrecision(SHT4X_HIGH_PRECISION);
    switch (sht4x.getPrecision()) {
        case SHT4X_HIGH_PRECISION:
        Serial.println("High precision");
        break;
        case SHT4X_MED_PRECISION:
        Serial.println("Med precision");
        break;
        case SHT4X_LOW_PRECISION:
        Serial.println("Low precision");
        break;
    }

    // You can have 6 different heater settings
    // higher heat and longer times uses more power
    // and reads will take longer too!
    sht4x.setHeater(SHT4X_NO_HEATER);
    switch (sht4x.getHeater()) {
        case SHT4X_NO_HEATER:
        Serial.println("No heater");
        break;
        case SHT4X_HIGH_HEATER_1S:
        Serial.println("High heat for 1 second");
        break;
        case SHT4X_HIGH_HEATER_100MS:
        Serial.println("High heat for 0.1 second");
        break;
        case SHT4X_MED_HEATER_1S:
        Serial.println("Medium heat for 1 second");
        break;
        case SHT4X_MED_HEATER_100MS:
        Serial.println("Medium heat for 0.1 second");
        break;
        case SHT4X_LOW_HEATER_1S:
        Serial.println("Low heat for 1 second");
        break;
        case SHT4X_LOW_HEATER_100MS:
        Serial.println("Low heat for 0.1 second");
        break;
    }

    Serial.println("\r\n******** SHT4x init ok *****\r\n");

    sensors_event_t humi_t, temp_t;
    for(;;) {
        uint32_t timestamp = millis();
        sht4x.getEvent(&humi_t, &temp_t);// populate temp and humidity objects with fresh data
        timestamp = millis() - timestamp;
        sht4x_data.humi = humi_t.relative_humidity;
        sht4x_data.temp = temp_t.temperature;
#ifdef SHT4X_DEBUG
        Serial.println("\r\n******** SHT4x **********\r\n");
        Serial.print("Temperature: "); Serial.print(temp_t.temperature); Serial.println(" degrees C");
        Serial.print("Humidity: "); Serial.print(humi_t.relative_humidity); Serial.println("% rH");

        Serial.print("Read duration (ms): ");
        Serial.println(timestamp);
        Serial.println("\r\n*************************\r\n");
#endif
        delay(1000);
    }
}
