#pragma once

#define SerialMon Serial

class SensorManager
{
private:
    float _temperature1 = 0.0f;
    float _humidity1 = 0.0f;
    float _pressure1 = 0.0f;

    float _temperature2 = 0.0f;
    float _humidity2 = 0.0f;
    float _pressure2 = 0.0f;

    float _temperature3 = 0.0f;
    float _humidity3 = 0.0f;
    float _pressure3 = 0.0f;

    float _lux = 0.0f;

    int _uvPrevSensorValue = 0;
    int _uvIntensity = 0;

    float _startAngle = 0.0f;
    int _correctedAngle = 0;

    float _windSpeed = 0.0f;
    float _rain = 0.0f;
    float _gust = 0.0f;

    float _batteryVoltage = 0.0f;

    void updateTemperatureHumidityPressure();
    void updateLux();
    void updateUvIntensity();
    void updateWindDirection();
    void updateSlave();
    void updateBatteryVoltage();

public:
    SensorManager();

    float getTemperature1() { return _temperature1; }
    float getHumidity1() { return _humidity1; }
    float getPressure1() { return _pressure1; }
    float getTemperature2() { return _temperature2; }
    float getHumidity2() { return _humidity2; }
    float getPressure2() { return _pressure2; }
    float getTemperature3() { return _temperature3; }
    float getHumidity3() { return _humidity3; }
    float getPressure3() { return _pressure3; }

    float getLux() { return _lux; }
    int getUvIntensity() { return _uvIntensity; }
    int getWindDirection() { return _correctedAngle; }
    float getWindSpeed() { return _windSpeed; }
    float getRain() { return _rain; }
    float getGust() { return _gust; }
    float getBatteryVoltage() { return _batteryVoltage; }

    void updateAll()
    {
        SerialMon.println("\n=================================== Sensors Status ===================================");

        updateTemperatureHumidityPressure();
        delay(10);

        updateLux();
        delay(10);

        updateUvIntensity();
        delay(10);

        updateWindDirection();
        delay(10);

        updateSlave();
        delay(10);

        updateBatteryVoltage();
        delay(10);
    }
};
