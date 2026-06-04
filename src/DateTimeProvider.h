#ifndef DATETIME_PROVIDER_H
#define DATETIME_PROVIDER_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

class DateTimeProvider
{
private:
    WiFiUDP _ntpUDP;
    NTPClient *_ntpClient = nullptr;

    const char *_ntpServer;
    long _timeOffset;
    unsigned long _updateInterval;
    unsigned long _lastSyncTime = 0;
    bool _isInitialized = false;

    void syncFromNTP();
    bool isTimeValid(unsigned long timestamp);

public:
    DateTimeProvider();
    DateTimeProvider(const char *ntpServer, long timeOffset, unsigned long updateInterval = 3600000);

    ~DateTimeProvider();

    bool begin(const char *ntpServer = "ru.pool.ntp.org",
               long timeOffset = 10800,
               unsigned long updateInterval = 3600000);

    void update();

    time_t now() { return ::now(); }
    int hour() { return ::hour(); }
    int minute() { return ::minute(); }
    int second() { return ::second(); }
    int day() { return ::day(); }
    int month() { return ::month(); }
    int year() { return ::year(); }
    int weekday() { return ::weekday(); }

    String getDateString(const char *separator = "-");
    String getTimeString(const char *separator = ":");
    String getDateTimeString(const char *dateSeparator = "-", const char *timeSeparator = ":");
    String getISO8601String();

    bool isTimeSet() const;
    bool isInitialized() const { return _isInitialized; }
    unsigned long getLastSyncTime() const { return _lastSyncTime; }

    void forceSync();
    void setUpdateInterval(unsigned long interval);

    long getTimeOffset() const { return _timeOffset; }
    void setTimeOffset(long offset);
    const char *getNTPServer() const { return _ntpServer; }
};

#endif