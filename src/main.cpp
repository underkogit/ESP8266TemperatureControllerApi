
#include "includes.h"

#define SDA_PIN D2
#define SCL_PIN D1

WebServerWrapper webServer;

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp280;
SensorHub sensorHub(SDA_PIN, SCL_PIN);

void connetedWiFi()
{
  Serial.println("=== ESP32-C3 WEB SERVER ===");
  Serial.print("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  const unsigned long timeout = 20000; // 20 s
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
    Serial.println("✗ Failed to connect to Wi-Fi.");
    Serial.print("Status: ");
    Serial.println(WiFi.status()); // выведет код состояния (6 = WL_CONNECT_FAILED и т.п.)
  }
}
void initServer()
{
  webServer.on("/api/sensors", HTTP_GET, []
               { webServer.sendJson(200, sensorHub.toJson()); });
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

  webServer.printRoutes(WiFi.localIP());
}

void loop()
{
  int v = analogRead(A0);
  static int minV = 1023, maxV = 0;
  if (v < minV)
    minV = v;
  if (v > maxV)
    maxV = v;
  Serial.printf("ADC=%d V=%.3f min=%d max=%d\n", v, v * (3.3 / 1023.0), minV, maxV);
  delay(100);
  webServer.handleClient();
}
