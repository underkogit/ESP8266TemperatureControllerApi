#include "sensor_hub.h"
#include <Wire.h>

SensorHub::SensorHub(int sda, int scl)
    : _sdaPin(sda),
      _sclPin(scl),
      _sht(),
      _aht(),
      _bmp(),
      _shtPresent(false),
      _ahtPresent(false),
      _bmpPresent(false),
      _shtAddr(""),
      _ahtAddr(""),
      _bmpAddr(""),
      _lastReadTime(0)
{
}

bool SensorHub::begin()
{
    _initializeWire();
    _detectSensors();

    if (_bmpPresent)
    {
        _configureBMP280();
    }

    Serial.println("=== Инициализация датчиков ===");
    if (_shtPresent)
    {
        Serial.println("✓ SHT31 найден на адресе " + _shtAddr);
    }
    else
    {
        Serial.println("✗ SHT31 не найден");
    }

    if (_ahtPresent)
    {
        Serial.println("✓ AHT20 найден");
    }
    else
    {
        Serial.println("✗ AHT20 не найден");
    }

    if (_bmpPresent)
    {
        Serial.println("✓ BMP280 найден на адресе " + _bmpAddr);
    }
    else
    {
        Serial.println("✗ BMP280 не найден");
    }
    Serial.println("================================");

    return _shtPresent || _ahtPresent || _bmpPresent;
}

void SensorHub::_initializeWire()
{
    if (_sdaPin >= 0 && _sclPin >= 0)
    {
#ifdef ARDUINO_ARCH_ESP32
        Wire.begin(_sdaPin, _sclPin);
#else
        Wire.begin();
#endif
    }
    else
    {
        Wire.begin();
    }

    delay(100);
}

void SensorHub::_detectSensors()
{

    if (_sht.begin(0x44))
    {
        _shtPresent = true;
        _shtAddr = "0x44";
    }

    if (_aht.begin())
    {
        _ahtPresent = true;
        _ahtAddr = "auto";
    }

    if (_bmp.begin(0x77))
    {
        _bmpPresent = true;
        _bmpAddr = "0x77";
    }
    else if (_bmp.begin(0x76))
    {
        _bmpPresent = true;
        _bmpAddr = "0x76";
    }
}

void SensorHub::_configureBMP280()
{
    _bmp.setSampling(
        Adafruit_BMP280::MODE_NORMAL,
        Adafruit_BMP280::SAMPLING_X2,
        Adafruit_BMP280::SAMPLING_X16,
        Adafruit_BMP280::FILTER_X16,
        Adafruit_BMP280::STANDBY_MS_500);
}

SensorReadings SensorHub::readAll()
{
    SensorReadings readings;
    readings.timestampMs = millis();

    _readSHT31(readings);
    _readAHT(readings);
    _readBMP280(readings);

    _lastReadings = readings;
    _lastReadTime = millis();

    return readings;
}

void SensorHub::_readSHT31(SensorReadings &readings)
{
    if (!_shtPresent)
        return;

    float temp, humidity;
    if (_sht.readBoth(&temp, &humidity))
    {
        if (_isValidTemperature(temp))
            readings.shtTemperature = temp;

        if (_isValidHumidity(humidity))
            readings.shtHumidity = humidity;
    }
}

void SensorHub::_readAHT(SensorReadings &readings)
{
    if (!_ahtPresent)
        return;

    sensors_event_t humidity, temp;

    if (_aht.getEvent(&humidity, &temp))
    {
        if (_isValidTemperature(temp.temperature))
            readings.ahtTemperature = temp.temperature;

        if (_isValidHumidity(humidity.relative_humidity))
            readings.ahtHumidity = humidity.relative_humidity;
    }
}

void SensorHub::_readBMP280(SensorReadings &readings)
{
    if (!_bmpPresent)
        return;

    float temp = _bmp.readTemperature();
    float pressure = _bmp.readPressure();

    if (_isValidTemperature(temp))
        readings.bmpTemperature = temp;

    if (_isValidPressure(pressure))
        readings.bmpPressure = pressure / 100.0f;
}

bool SensorHub::_isValidTemperature(float temp) const
{
    return !isnan(temp) && temp > -50.0f && temp < 125.0f;
}

bool SensorHub::_isValidHumidity(float humidity) const
{
    return !isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f;
}

bool SensorHub::_isValidPressure(float pressure) const
{
    return !isnan(pressure) && pressure > 30000.0f && pressure < 110000.0f;
}

SensorStatus SensorHub::getSHTStatus() const
{
    SensorStatus status;
    status.present = _shtPresent;
    status.address = _shtAddr;
    status.type = _shtPresent ? "SHT31" : "";
    return status;
}

SensorStatus SensorHub::getAHTStatus() const
{
    SensorStatus status;
    status.present = _ahtPresent;
    status.address = _ahtAddr;
    status.type = _ahtPresent ? "AHT20" : "";
    return status;
}

SensorStatus SensorHub::getBMPStatus() const
{
    SensorStatus status;
    status.present = _bmpPresent;
    status.address = _bmpAddr;
    status.type = _bmpPresent ? "BMP280" : "";
    return status;
}

String SensorHub::toJson(String time) const
{

    SensorReadings readings = const_cast<SensorHub *>(this)->readAll();

    JsonDocument doc;

#define SET_SENSOR(name, present, temp, hum, press)                       \
    if (!present)                                                         \
        doc["sensors"][name]["error"] = #name " not found";               \
    else                                                                  \
    {                                                                     \
        if (!isnan(temp))                                                 \
        {                                                                 \
            doc["sensors"][name]["temperature"]["value"] = temp;          \
            doc["sensors"][name]["temperature"]["unit"] = "°C";           \
        }                                                                 \
        else                                                              \
            doc["sensors"][name]["temperature"]["error"] = "read failed"; \
        if (!isnan(hum))                                                  \
        {                                                                 \
            doc["sensors"][name]["humidity"]["value"] = hum;              \
            doc["sensors"][name]["humidity"]["unit"] = "%";               \
        }                                                                 \
        else                                                              \
            doc["sensors"][name]["humidity"]["error"] = "read failed";    \
        if (!isnan(press))                                                \
        {                                                                 \
            doc["sensors"][name]["pressure"]["value"] = press;            \
            doc["sensors"][name]["pressure"]["unit"] = "гПа";             \
        }                                                                 \
    }

    SET_SENSOR("sht31", _shtPresent, readings.shtTemperature, readings.shtHumidity, NAN);
    SET_SENSOR("aht20", _ahtPresent, readings.ahtTemperature, readings.ahtHumidity, NAN);
    SET_SENSOR("bmp280", _bmpPresent, readings.bmpTemperature, NAN, readings.bmpPressure);

    doc["datetime"] = readings.timestampMs;
    doc["timestamp"] = time;

    String json;
    serializeJsonPretty(doc, json);
    return json;
}
