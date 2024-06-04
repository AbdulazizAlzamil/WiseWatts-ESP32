#ifndef DATA_RETRIEVAL_H
#define DATA_RETRIEVAL_H

#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <vector>

extern const char* EXPRESS_SERVER_IP;

extern std::vector<int> relayPinNumbers;
extern std::vector<int> sensorPinNumbers;
extern std::vector<bool> states;
extern std::vector<int> socketIds;
extern int socketId;

void retrieveSocketsData(int serialNumber);
void retrieveBoardCommands(int serialNumber);
void sendCurrentData(int socketId, float current);
void sendHourlyConsumption(float consumptionValue, int socketId, int userId);

#endif  // DATA_RETRIEVAL_H
