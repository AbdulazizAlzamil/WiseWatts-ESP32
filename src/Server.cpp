#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <DataRetrieval.h>
#include "WiFiGeneric.h"
#include "esp_wifi_types.h"
#include "string"

AsyncWebServer server(80);

String chipId = String(ESP.getEfuseMac(), DEC);
long intChipId = chipId.toInt();

const char* apSSID = "ESP32_AP";
const char* apPassword = "esp32password";
// const char *ssid = "Wokwi-GUEST";
// const char *password = "";
// const int WIFI_CHANNEL = 6; // Speeds up the connection in Wokwi
const char *ssid = "calma space 5g";
const char *password = "calmaspace55555";

std::vector<int> relayPinNumbers;
std::vector<int> sensorPinNumbers;
std::vector<bool> states;
std::vector<int> socketIds;
int socketId;

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void setup() {
    Serial.begin(9600);
    pinMode(26, OUTPUT);

    Serial.print("Connecting to WiFi... ");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apSSID, apPassword);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    Serial.print("Chip ID: ");
    Serial.println(intChipId);

    retrieveSocketsData(intChipId);

    for (size_t i = 0; i < relayPinNumbers.size(); i++) {
        int pinNumber = relayPinNumbers[i];
        bool state = states[i];
        digitalWrite(pinNumber, state);
    }

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("New client connected");
        // registerToServer();
    });

    server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebParameter *ssid = request->getParam("ssid", true);
        AsyncWebParameter *password = request->getParam("password", true);
        Serial.println("Received SSID: " + ssid->value());
        Serial.println("Received Password: " + password->value());
        request->send(200, "text/plain", "Connected to network");
        WiFi.begin(ssid->value().c_str(), password->value().c_str());
    });

    server.onNotFound(notFound);
    server.begin();
    Serial.println("HTTP server started (http://localhost:8180)");
}


void loop()
{
    // retrieveBoardCommands(intChipId);

    for(int i = 0; i < sensorPinNumbers.size(); i++) {
        ACS712 sensor(ACS712_20A, sensorPinNumbers[i]);
        float current = sensor.getCurrentAC();
        Serial.println(current);
        sendCurrentData(socketIds[i], current);
    }

    delay(2000);
}
