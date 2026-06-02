#pragma once

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif

#include <vector>
#include <ArduinoJson.h>
class WebServerWrapper
{
private:
    ESP8266WebServer server;

    struct ApiRoute
    {
        String path;
        HTTPMethod method;
        ESP8266WebServer::THandlerFunction handler;
    };

    std::vector<ApiRoute> routes;

public:
    WebServerWrapper(int port = 80) : server(port) {}

    void on(const char *path, HTTPMethod method, ESP8266WebServer::THandlerFunction handler)
    {

        routes.push_back({String(path), method, handler});

        server.on(path, method, handler);
    }

    void sendJson(int code, const String &json)
    {
        server.sendHeader("Content-Type", "application/json; charset=utf-8");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(code, "application/json", json);
    }

    void printRoutes(IPAddress ip)
    {
        Serial.println("\n=== Registered API Routes ===");
        for (size_t i = 0; i < routes.size(); i++)
        {
            String method = getMethodName(routes[i].method);
            Serial.printf("[%d] %s http://%s%s\n",
                          i,
                          method.c_str(),
                          ip.toString().c_str(),
                          routes[i].path.c_str());
        }
    }

    String getRoutesAsJson()
    {
        StaticJsonDocument<512> doc;
        JsonArray routesArray = doc.createNestedArray("routes");

        for (size_t i = 0; i < routes.size(); i++)
        {
            JsonObject route = routesArray.createNestedObject();
            route["id"] = i;
            route["method"] = getMethodName(routes[i].method);
            route["path"] = routes[i].path;
        }

        doc["total"] = routes.size();

        String json;
        serializeJson(doc, json);
        return json;
    }

    const std::vector<ApiRoute> &getRoutes() const
    {
        return routes;
    }

    size_t getRoutesCount() const
    {
        return routes.size();
    }

    void printRoute(size_t index)
    {
        if (index < routes.size())
        {
            String method = getMethodName(routes[index].method);
            Serial.printf("Route[%d]: %s %s\n", index, method.c_str(), routes[index].path.c_str());
        }
        else
        {
            Serial.printf("Route[%d]: Not found (total: %d)\n", index, routes.size());
        }
    }

    void findRoutesByPath(const String &pathPattern)
    {
        Serial.printf("\n=== Routes containing '%s' ===\n", pathPattern.c_str());
        int found = 0;
        for (size_t i = 0; i < routes.size(); i++)
        {
            if (routes[i].path.indexOf(pathPattern) >= 0)
            {
                String method = getMethodName(routes[i].method);
                Serial.printf("[%d] %s %s\n", i, method.c_str(), routes[i].path.c_str());
                found++;
            }
        }
        Serial.printf("Found: %d\n\n", found);
    }

    void begin()
    {
        server.begin();
       
    }

    void handleClient()
    {
        server.handleClient();
    }

private:
    String getMethodName(HTTPMethod method)
    {
        switch (method)
        {
        case HTTP_GET:
            return "GET";
        case HTTP_POST:
            return "POST";
        case HTTP_PUT:
            return "PUT";
        case HTTP_DELETE:
            return "DELETE";
        case HTTP_PATCH:
            return "PATCH";
        case HTTP_HEAD:
            return "HEAD";
        case HTTP_OPTIONS:
            return "OPTIONS";
        default:
            return "OTHER";
        }
    }
};