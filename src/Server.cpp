#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <api_requests.h>
#include "WiFiGeneric.h"
#include "esp_wifi_types.h"
#include "string"
#include "ACS712.h"

// Global variables
bool relay = false;
float hourlyConsumption = 0;
float seconds = 0;
AsyncWebServer server(80);

// Convert MAC address to String
String getChipId() {
    uint64_t chipid = ESP.getEfuseMac();
    uint32_t low = chipid & 0xFFFFFF;
    uint32_t high = (chipid >> 24) & 0xFFFFFF;
    return String(high) + String(low);
}

String chipId = getChipId();
long intChipId = chipId.toInt();

const char* apSSID = "ESP32_AP";
const char* apPassword = "esp32password";
int relayPinNumber;

const char *ssid = "Airbox-CD27";
const char *password = "S425Z754";

std::vector<int> relayPinNumbers;
std::vector<bool> states;
std::vector<int> socketIds;
std::vector<int> sensorPinNumbers;
std::vector<ACS712> sensors;

// Handle 404 Not Found
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    pinMode(26, OUTPUT);
    Serial.begin(115200);

    // Connect to WiFi
    Serial.print("Connecting to WiFi... ");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apSSID, apPassword);
    WiFi.begin(ssid, password);

    int retries = 10;
    while (WiFi.status() != WL_CONNECTED && retries > 0) {
        delay(1000);
        Serial.print(".");
        retries--;
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Failed!");
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
    for (size_t i = 0; i < socketIds.size(); i++) {
        ACS712 sensor(ACS712_20A, sensorPinNumbers[i]);
        sensor.calibrate();
        sensors.push_back(sensor);
        int pinNumber = relayPinNumbers[i];
        bool state = states[i];
        digitalWrite(pinNumber, state);
        relay = state;
    }

    // Event handler for new client connections
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("New client connected");
        // registerToServer();
    });

    // Handle POST requests to /connect endpoint
    server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebParameter *ssidParam = request->getParam("ssid", true);
        AsyncWebParameter *passwordParam = request->getParam("password", true);
        Serial.println("Received SSID: " + ssidParam->value());
        Serial.println("Received Password: " + passwordParam->value());
        request->send(200, "text/plain", "Connected to network");
        WiFi.begin(ssidParam->value().c_str(), passwordParam->value().c_str());
    });

    // Handle 404 Not Found
    server.onNotFound(notFound);

    // Start the server
    server.begin();
}

void loop() {
    seconds += 0.5;

    // Retrieve commands from the server
    retrieveBoardCommands(intChipId);

    // Process each sensor
    for (int i = 0; i < sensorPinNumbers.size(); i++) {
        ACS712 sensor = sensors[i];
        float current = sensor.getCurrentAC();
        current -= 0.7;
        Serial.print("\n\nCurrent before: ");
        Serial.print(current);
        if (current < 0.5) {
            current = 0;
        }
        Serial.print("\n\nCurrent before mult: ");
        Serial.println(current);
        // Print current reading based on the relay state
        if (states[i]) {
            Serial.println(String("I = ") + (current * 0.04446854663) + " A");
            current *= 0.04446854663;
            Serial.print("\n\nCurrent after mult: ");
            Serial.println(current);
            if(fabs(seconds - round(seconds)) < 0.000001) {
                hourlyConsumption += current;
                Serial.print("Seconds: ");
                Serial.println(seconds);
                
                Serial.print("\n\nhourlyConsumption: ");
                Serial.print(hourlyConsumption);
            }

            if(seconds >= 4) {
                // Save it in the database
                sendHourlyConsumption(hourlyConsumption, socketIds[i], 21);
                seconds = hourlyConsumption = 0;
            }
        } else {
            Serial.println(String("I = ") + (current) + " A");
        }

        // Send current data to the server
        sendCurrentData(socketIds[i], current);
    }

    Serial.print("\n\nWiFi status: ");
    Serial.print(WiFi.status());

    delay(500);
}












// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <WiFiClient.h>
// #include <ArduinoJson.h>
// #include <DataRetrieval.h>
// #include "WiFiGeneric.h"
// #include "esp_wifi_types.h"
// #include "string"
// // #include "FS.h"
// #include "ACS712.h"

// // Global variables
// bool relay = false;
// AsyncWebServer server(80);

// String chipId = String(ESP.getEfuseMac(), DEC);
// long intChipId = chipId.toInt();

// const char* apSSID = "ESP32_AP";
// const char* apPassword = "esp32password";
// int relayPinNumber;

// const char *ssid = "Orange-5FD9";
// const char *password = "21070044";

// std::vector<int> relayPinNumbers;
// std::vector<bool> states;
// std::vector<int> socketIds;
// std::vector<int> sensorPinNumbers;
// std::vector<ACS712> sensors;

// // Handle 404 Not Found
// void notFound(AsyncWebServerRequest *request) {
//     request->send(404, "text/plain", "Not found");
// }

// void setup() {
//     pinMode(26, OUTPUT);
//     Serial.begin(115200);

   

//     delay(1000);

//     // Connect to WiFi
//     Serial.print("Connecting to WiFi... ");
//     WiFi.mode(WIFI_AP_STA);
//     WiFi.softAP(apSSID, apPassword);
//     WiFi.begin(ssid, password);
    
//     if (WiFi.waitForConnectResult() != WL_CONNECTED) {
//         Serial.printf("WiFi Failed!\n");
//         return;
//     }

//     Serial.println("Connected!");
//     Serial.print("IP Address: ");
//     Serial.println(WiFi.localIP());

//     IPAddress IP = WiFi.softAPIP();
//     Serial.print("AP IP address: ");
//     Serial.println(IP);

//     Serial.print("Chip ID: ");
//     Serial.println(intChipId);


//     retrieveSocketsData(intChipId);
//     // Set initial relay states
//     for (size_t i = 0; i < relayPinNumbers.size(); i++) {
//         int pinNumber = relayPinNumbers[i];
//         bool state = states[i];
//         digitalWrite(pinNumber, state);
//         relay = state;
//     }

//  // Initialize sensors
//     for(int i = 0; i < sensorPinNumbers.size(); i++) {
//         ACS712 sensor(ACS712_20A, sensorPinNumbers[i]);
//         Serial.println("Calibrating... No current should flow through the sensor at this moment");
//         sensor.calibrate();
//         Serial.println("Done!");
//         sensors.push_back(sensor);
//     }
//     // Event handler for new client connections
//     WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
//         Serial.println("New client connected");
//         // registerToServer();
//     });

//     // Handle POST requests to /connect endpoint
//     server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
//         AsyncWebParameter *ssidParam = request->getParam("ssid", true);
//         AsyncWebParameter *passwordParam = request->getParam("password", true);
//         Serial.println("Received SSID: " + ssidParam->value());
//         Serial.println("Received Password: " + passwordParam->value());
//         request->send(200, "text/plain", "Connected to network");
//         WiFi.begin(ssidParam->value().c_str(), passwordParam->value().c_str());
//     });

//     // Handle 404 Not Found
//     server.onNotFound(notFound);

//     // Start the server
//     server.begin();
// }

// void loop() {
//     // Retrieve commands from the server
//     retrieveBoardCommands(intChipId);

//     // Process each sensor
//     for(int i = 0; i < sensorPinNumbers.size(); i++) {
//         ACS712 sensor = sensors[i];
//         float current = sensor.getCurrentAC();
//         current -= 0.7;

//         if (current < 0.3) {
//             current = 0;
//         }

//         // Print current reading based on the relay state
//         if(states[i]){
//             Serial.println(String("I = ") + (current * 0.04446854663) + " A");
//         } else {
//             Serial.println(String("I = ") + (current) + " A");
//         }

//         // Send current data to the server
//         sendCurrentData(socketIds[i], current);
//     }

//     delay(1000);
// }
