#include "DateTimeProvider.h"

DateTimeProvider::DateTimeProvider()
{
    _ntpServer = "ru.pool.ntp.org";
    _timeOffset = 10800;
    _updateInterval = 3600000;
    _ntpClient = nullptr;
}

DateTimeProvider::DateTimeProvider(const char *ntpServer, long timeOffset, unsigned long updateInterval)
{
    _ntpServer = ntpServer;
    _timeOffset = timeOffset;
    _updateInterval = updateInterval;
    _ntpClient = nullptr;
}

DateTimeProvider::~DateTimeProvider()
{
    if (_ntpClient != nullptr)
    {
        delete _ntpClient;
        _ntpClient = nullptr;
    }
}

bool DateTimeProvider::begin(const char *ntpServer, long timeOffset, unsigned long updateInterval)
{

    _ntpServer = ntpServer;
    _timeOffset = timeOffset;
    _updateInterval = updateInterval;

    if (_ntpClient != nullptr)
    {
        delete _ntpClient;
    }
    _ntpClient = new NTPClient(_ntpUDP, _ntpServer, _timeOffset, _updateInterval);

    if (_ntpClient == nullptr)
    {
        Serial.println("DateTimeProvider: Failed to create NTPClient");
        _isInitialized = false;
        return false;
    }

    _ntpClient->begin();

    syncFromNTP();

    _isInitialized = true;
    return _lastSyncTime > 0;
}

void DateTimeProvider::syncFromNTP()
{
    if (_ntpClient == nullptr)
        return;

    _ntpClient->update();

    if (_ntpClient->isTimeSet())
    {
        unsigned long epochTime = _ntpClient->getEpochTime();
        if (isTimeValid(epochTime))
        {
            setTime(epochTime);
            _lastSyncTime = millis();

            Serial.print("DateTimeProvider: Synced time = ");
            Serial.println(getISO8601String());
        }
        else
        {
            Serial.println("DateTimeProvider: Received invalid timestamp");
        }
    }
    else
    {
        Serial.println("DateTimeProvider: NTP sync failed");
    }
}

bool DateTimeProvider::isTimeValid(unsigned long timestamp)
{

    return timestamp > 1577836800 && timestamp < 4102444800;
}

void DateTimeProvider::update()
{
    if (!_isInitialized || _ntpClient == nullptr)
        return;

    if (millis() - _lastSyncTime >= _updateInterval)
    {
        syncFromNTP();
    }
}

void DateTimeProvider::forceSync()
{
    syncFromNTP();
}

void DateTimeProvider::setUpdateInterval(unsigned long interval)
{
    _updateInterval = interval;
    if (_ntpClient != nullptr)
    {
        _ntpClient->setUpdateInterval(interval);
    }
}

void DateTimeProvider::setTimeOffset(long offset)
{
    _timeOffset = offset;
    if (_ntpClient != nullptr)
    {
        _ntpClient->setTimeOffset(offset);
    }

    forceSync();
}

String DateTimeProvider::getDateString(const char *separator)
{
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%04d%s%02d%s%02d",
             year(), separator, month(), separator, day());
    return String(buffer);
}

String DateTimeProvider::getTimeString(const char *separator)
{
    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%02d%s%02d%s%02d",
             hour(), separator, minute(), separator, second());
    return String(buffer);
}

String DateTimeProvider::getDateTimeString(const char *dateSeparator, const char *timeSeparator)
{
    return getDateString(dateSeparator) + " " + getTimeString(timeSeparator);
}

String DateTimeProvider::getISO8601String()
{
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d+03:00",
             year(), month(), day(), hour(), minute(), second());
    return String(buffer);
}

bool DateTimeProvider::isTimeSet() const
{

    return ::year() > 2020 && ::year() < 2100;
}