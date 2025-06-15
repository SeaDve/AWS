#include <Adafruit_BME280.h>
#include <Adafruit_INA219.h>
#include <BH1750.h>

#include "SensorManager.h"

#define SerialMon Serial

const uint8_t UV_PIN = 32;

const int BME_WIRE_ADDR = 0x70;
const int AS5600_ADDR = 0x36;
const int SLAVE_ADDR = 0x08;

const int RAIN_TIP_VALUE = 0.1099;
const float CALIBRATION_FACTOR = 2.4845;
const float RADIUS = 0.05;
const float CIRCUMFERENCE = 2 * PI * RADIUS * CALIBRATION_FACTOR;
const int PERIOD = 60;

Adafruit_BME280 bme1;
Adafruit_BME280 bme2;
Adafruit_BME280 bme3;

BH1750 lightMeter;

Adafruit_INA219 ina219;

RTC_DATA_ATTR float rtcStartAngle;
RTC_DATA_ATTR int rtcCorrectedAngle;

SensorManager::SensorManager()
{
}

void selectBmeBus(uint8_t bus)
{
    Wire.beginTransmission(BME_WIRE_ADDR);
    Wire.write(1 << bus);
    Wire.endTransmission();
}

void updateTemperatureHumidityPressureInner(const char *name, Adafruit_BME280 bme, int bus, float *temp, float *hum, float *pres)
{
    // BME280 Connect
    SerialMon.printf("BME %s: ", name);

    selectBmeBus(bus);

    if (!bme.begin(0x76))
    {
        SerialMon.println(" Failed");
        return;
    }

    SerialMon.println(" OK");
    *temp = bme.readTemperature();
    *hum = bme.readHumidity();
    *pres = bme.readPressure() / 100.0F;
}

void SensorManager::updateTemperatureHumidityPressure()
{
    updateTemperatureHumidityPressureInner("1", bme1, 2, &_temperature1, &_humidity1, &_pressure1);
    delay(10);

    updateTemperatureHumidityPressureInner("2", bme2, 3, &_temperature2, &_humidity2, &_pressure2);
    delay(10);

    updateTemperatureHumidityPressureInner("3", bme3, 4, &_temperature3, &_humidity3, &_pressure3);
}

void SensorManager::updateLux()
{
    SerialMon.print("BH1750: ");

    if (!lightMeter.begin())
    {
        SerialMon.println(" Failed");
        return;
    }

    SerialMon.println(" OK");

    _lux = lightMeter.readLightLevel();
}

void SensorManager::updateUvIntensity()
{
    const int DEBOUNCE_THRESHOLD = 10;

    SerialMon.print("UV: ");

    analogReadResolution(12);
    int sensorValue = analogRead(UV_PIN);

    if (abs(sensorValue - _uvPrevSensorValue) > DEBOUNCE_THRESHOLD)
    {
        _uvPrevSensorValue = sensorValue;
        delay(50);
        sensorValue = analogRead(UV_PIN);
    }

    if (abs(sensorValue - _uvPrevSensorValue) <= DEBOUNCE_THRESHOLD)
    {
        _uvPrevSensorValue = sensorValue;
        SerialMon.println(" OK");

        float sensorVoltage = sensorValue * (3.3 / 4095.0);
        _uvIntensity = sensorVoltage * 1000;
    }
    else
    {
        SerialMon.println(" Failed");
        return;
    }
}

float getDegAngleFromRaw()
{
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(0x0D);
    Wire.endTransmission();
    Wire.requestFrom(AS5600_ADDR, 1);
    int lowbyte = Wire.read();

    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(0x0C);
    Wire.endTransmission();
    Wire.requestFrom(AS5600_ADDR, 1);
    word highbyte = Wire.read();

    highbyte = highbyte << 8;
    uint16_t rawAngle = highbyte | lowbyte;
    float degAngle = rawAngle * 0.087890625;

    return degAngle;
}

int getCorrectedAngle(float degAngle, float startAngle)
{
    int correctedAngle = 360 - degAngle + startAngle;
    if (correctedAngle > 360)
    {
        correctedAngle -= 360;
    }
    rtcCorrectedAngle = correctedAngle;
    if (correctedAngle == 360)
    {
        correctedAngle = 0;
    }
    return correctedAngle;
}

void SensorManager::updateWindDirection()
{
    SerialMon.print("AS5600: ");

    _startAngle - rtcStartAngle;
    float degAngle = getDegAngleFromRaw();

    if (rtcStartAngle == 0)
    {
        rtcStartAngle = degAngle;
    }

    _startAngle = rtcStartAngle;
    _correctedAngle = rtcCorrectedAngle;

    Wire.beginTransmission(AS5600_ADDR);
    if (Wire.endTransmission() != 0)
    {
        SerialMon.println(" Failed");
        return;
    }

    SerialMon.println(" OK");
    degAngle = getDegAngleFromRaw();
    _correctedAngle = getCorrectedAngle(degAngle, _startAngle);
}

void SensorManager::updateSlave()
{
    SerialMon.print("Slave: ");

    Wire.beginTransmission(SLAVE_ADDR);
    if (Wire.endTransmission() != 0)
    {
        SerialMon.println(" Failed");
        return;
    }

    SerialMon.println(" OK");

    Wire.requestFrom(SLAVE_ADDR, 6);

    // Rain
    if (Wire.available() >= 4)
    {
        byte msb = Wire.read();
        byte lsb = Wire.read();
        uint16_t receivedRainCount = (msb << 8) | lsb;
        _rain = receivedRainCount * RAIN_TIP_VALUE;
    }

    // Wind speed
    if (Wire.available() >= 2)
    {
        byte msb = Wire.read();
        byte lsb = Wire.read();
        uint16_t receivedWindSpeedCount = (msb << 8) | lsb;
        _windSpeed = ((CIRCUMFERENCE * receivedWindSpeedCount * 3.6) / PERIOD);
    }

    // Gust
    if (Wire.available() >= 2)
    {
        byte msb = Wire.read();
        byte lsb = Wire.read();
        uint16_t receivedGustCount = (msb << 8) | lsb;
        _gust = ((CIRCUMFERENCE * receivedGustCount * 3.6) / 3);
    }
}

void SensorManager::updateBatteryVoltage()
{
    SerialMon.print("INA219: ");

    if (!ina219.begin())
    {
        SerialMon.println(" Failed");
        return;
    }

    SerialMon.println(" OK");

    _batteryVoltage = ina219.getBusVoltage_V();
}
