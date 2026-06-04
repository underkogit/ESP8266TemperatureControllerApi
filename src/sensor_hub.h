#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Adafruit_SHT31.h"
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

struct SensorReadings
{
  float shtTemperature;
  float shtHumidity;
  float ahtTemperature;
  float ahtHumidity;
  float bmpTemperature;
  float bmpPressure;

  unsigned long timestampMs;

  SensorReadings()
      : shtTemperature(NAN), shtHumidity(NAN),
        ahtTemperature(NAN), ahtHumidity(NAN),
        bmpTemperature(NAN), bmpPressure(NAN),
        timestampMs(0) {}
};

struct SensorStatus
{
  bool present;
  String address;
  String type;
};

class SensorHub
{
public:
  SensorHub(int sda = -1, int scl = -1);
  ~SensorHub() = default;

  // Инициализация
  bool begin();

  // Чтение данных
  SensorReadings readAll();

  // Информация о датчиках
  SensorStatus getSHTStatus() const;
  SensorStatus getAHTStatus() const;
  SensorStatus getBMPStatus() const;

  // JSON вывод
  String toJson( String time ) const;
  
  // Проверка наличия датчиков
  bool isSHTPresent() const { return _shtPresent; }
  bool isAHTPresent() const { return _ahtPresent; }
  bool isBMPPresent() const { return _bmpPresent; }

private:
  // Конфигурация
  int _sdaPin;
  int _sclPin;

  // Экземпляры датчиков
  Adafruit_SHT31 _sht;
  Adafruit_AHTX0 _aht;
  Adafruit_BMP280 _bmp;

  // Статус датчиков
  bool _shtPresent;
  bool _ahtPresent;
  bool _bmpPresent;

  // Адреса датчиков
  String _shtAddr;
  String _ahtAddr;
  String _bmpAddr;

  // Кэш последних показаний
  mutable SensorReadings _lastReadings;
  mutable unsigned long _lastReadTime;

  // Приватные методы
  void _initializeWire();
  void _detectSensors();
  void _configureBMP280();
  void _readSHT31(SensorReadings &readings);
  void _readAHT(SensorReadings &readings);
  void _readBMP280(SensorReadings &readings);
  bool _isValidTemperature(float temp) const;
  bool _isValidHumidity(float humidity) const;
  bool _isValidPressure(float pressure) const;
};
