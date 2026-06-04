
#include "includes.h"

#define SDA_PIN D2
#define SCL_PIN D1

WebServerWrapper webServer;

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp280;
SensorHub sensorHub(SDA_PIN, SCL_PIN);

DateTimeProvider dtProvider;
void connetedWiFi()
{
  Serial.println("=== WEB SERVER ===");
  Serial.print("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  const unsigned long timeout = 20000;
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout)
  {
    Serial.print('.');
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("Failed to connect to Wi-Fi.");
    Serial.print("Status: ");
    Serial.println(WiFi.status());
  }
}
void initServer()
{
  webServer.on("/api/sensors", HTTP_GET, []
               { 
                  dtProvider.update();
                webServer.sendJson(200, sensorHub.toJson(dtProvider.getDateTimeString("-", ":"))); });
  webServer.on("/api/status", HTTP_GET, []
               {
                 {
  StaticJsonDocument<256> doc;
  doc["status"] = "online";
  doc["uptime"] = millis();

  String json;
  serializeJson(doc, json);

  webServer.sendJson(200, json);
                 } });

  webServer.begin();
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  connetedWiFi();
  initServer();

  sensorHub.begin();
  if (dtProvider.begin("ru.pool.ntp.org", 10800, 3600000))
  {
    Serial.println("DateTimeProvider initialized successfully");
  }
  else
  {
    Serial.println("DateTimeProvider initialization failed!");
  }
  webServer.printRoutes(WiFi.localIP());
}

void loop()
{

  webServer.handleClient();
}
