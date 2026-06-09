#include "includes.h"

#define SDA_PIN D2
#define SCL_PIN D1
#define WIFI_CHECK_INTERVAL 5000

WebServerWrapper webServer;
Adafruit_SHT31 sht31;
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp280;
SensorHub sensorHub(SDA_PIN, SCL_PIN);
DateTimeProvider dtProvider;

unsigned long lastWiFiCheck = 0;

void connectWiFi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000)
  {
    Serial.print('.');
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
    Serial.println("\nMAC: " + WiFi.macAddress());
  }
  else
  {
    Serial.println("\nWiFi connection failed!");
  }
}

void initServer()
{
  webServer.on("/", HTTP_GET, []
               { webServer.sendHtml(200, index_html); });
  webServer.on("/manifest.json", HTTP_GET, []
               { webServer.send(200, "application/json", index_maniest); });
  webServer.on("/api/sensors", HTTP_GET, []
               { 
    dtProvider.update();
    webServer.sendJson(200, sensorHub.toJson(dtProvider.getDateTimeString("-", ":"))); });

  webServer.on("/icons/icon-512.png", HTTP_GET, []()
               { webServer.send_P(200, "image/png", icon512, sizeof(icon512)); });

  webServer.on("/icons/icon-512.png", HTTP_GET, []()
               { webServer.send_P(200, "image/png", icon512, sizeof(icon512)); });

  webServer.on("/icons/icon-192.png", HTTP_GET, []()
               { webServer.send_P(200, "image/png", icon192, sizeof(icon192)); });

  webServer.on("/icons/icon-192.png", HTTP_GET, []()
               { webServer.send_P(200, "image/png", icon192, sizeof(icon192)); });

  webServer.on("/api/status", HTTP_GET, []
               {
    StaticJsonDocument<256> doc;
    doc["status"] = WiFi.status() == WL_CONNECTED ? "online" : "offline";
    doc["IP"] = WiFi.localIP().toString();
    doc["MAC"] = WiFi.macAddress();
    doc["uptime"] = millis();
    
    String json;
    serializeJson(doc, json);
    webServer.sendJson(200, json); });

  webServer.begin();
}

void checkWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nWiFi lost! Reconnecting...");
    connectWiFi();
    if (WiFi.status() == WL_CONNECTED)
      initServer();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP Sensors Device ===");

  connectWiFi();
  sensorHub.begin();
  dtProvider.begin("ru.pool.ntp.org", 10800, 3600000);
  initServer();
  webServer.printRoutes(WiFi.localIP());
  Serial.println("Open browser at: http://" + WiFi.localIP().toString());
}

void loop()
{
  if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL)
  {
    lastWiFiCheck = millis();
    checkWiFi();
  }
  webServer.handleClient();
  delay(1);
}