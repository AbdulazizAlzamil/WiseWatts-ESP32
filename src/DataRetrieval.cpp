#include "DataRetrieval.h"
#include <Arduino.h>

const char* EXPRESS_SERVER_IP = "192.168.1.109";

void retrieveSocketsData(int serialNumber) {
    WiFiClient client;
    if (!client.connect(EXPRESS_SERVER_IP, 3000)) {
        Serial.println("Connection to server failed!");
        return;
    }

    Serial.println("Connected to server");

    client.print("GET /WisewattsDeviceController/FindDeviceBySerialNumber/");
    client.print(serialNumber);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(EXPRESS_SERVER_IP);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    String response;

    while (client.connected()) {
        if (client.available()) {
            response = client.readString();
        }
    }

    Serial.print("Response: ");
    Serial.println(response);

    // DynamicJsonDocument doc(1024);
    // deserializeJson(doc, response);

    // Serial.print("Doc: ");
    // Serial.println(doc.to<JsonArray>());

    // JsonArray data = doc.as<JsonArray>();
    // Serial.print("Data: ");
    // Serial.println(data);


    // int deviceId;
    // if (data.size() > 0) {
    //     JsonObject obj = data[0];
    //     deviceId = obj["device_id"];
    //     Serial.print("Device ID: ");
    //     Serial.println(deviceId);
    // }

    int deviceId;
    int deviceIdIndex = response.indexOf("\"device_id\"");
    if (deviceIdIndex != -1) {
        int colonIndex = response.indexOf(":", deviceIdIndex);
        int commaIndex = response.indexOf(",", colonIndex);

        String deviceIdString = response.substring(colonIndex + 1, commaIndex);

        deviceId = deviceIdString.toInt();

        Serial.print("Device ID: ");
        Serial.println(deviceId);
    } else {
        Serial.println("Device ID not found in response");
    }

    client.stop();

    //////////////////////////////////////////////////////////////

    if (!client.connect(EXPRESS_SERVER_IP, 3000)) {
        Serial.println("Connection to server failed!");
        return;
    }

    client.print("GET /SocketController/FindSocketsByDevice/");
    client.print(deviceId);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(EXPRESS_SERVER_IP);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    String response2;

    while (client.connected()) {
        if (client.available()) {
            response2 = client.readString();
        }
    }

    Serial.print("Response 2: ");
    Serial.println(response2);

    client.stop();
    
    // DynamicJsonDocument doc2(1024);
    // deserializeJson(doc2, response2);

    // Serial.print("Doc 2: ");
    // Serial.println(doc2.as<JsonArray>());

    // JsonArray data2 = doc2.as<JsonArray>();
    // for (JsonObject socket : data2) {
    //     int pinNumber = socket["pin_number"];
    //     bool state = socket["state"];
    //     int socketId = socket["socket_id"];
    //     relayPinNumbers.push_back(pinNumber);
    //     states.push_back(state);
    //     socketIds.push_back(socketId);
    // }

    int startIndex = response2.indexOf("[");
    int endIndex = response2.lastIndexOf("]");
    String jsonArrayString = response2.substring(startIndex, endIndex + 1);

    int pos = jsonArrayString.indexOf('{'); // Find the position of the first object in the array
    while (pos != -1) {
        int nextPos = jsonArrayString.indexOf('{', pos + 1);
        if (nextPos == -1) {
            nextPos = jsonArrayString.length(); // If no more objects found, set nextPos to the end of the string
        }
        String jsonObjectString = jsonArrayString.substring(pos, nextPos);

        int socketId = jsonObjectString.substring(jsonObjectString.indexOf("socket_id") + 11, jsonObjectString.indexOf(",", jsonObjectString.indexOf("socket_id"))).toInt();
        int pinNumber = jsonObjectString.substring(jsonObjectString.indexOf("pin_number") + 12, jsonObjectString.indexOf(",", jsonObjectString.indexOf("pin_number"))).toInt();
        bool state = jsonObjectString.substring(jsonObjectString.indexOf("state") + 7, jsonObjectString.indexOf(",", jsonObjectString.indexOf("state"))).equals("true");
        int sensorPinNumber = jsonObjectString.substring(jsonObjectString.indexOf("sensor_pin_number") + 19, jsonObjectString.indexOf(",", jsonObjectString.indexOf("sensor_pin_number"))).toInt();

        Serial.print("Socket ID: ");
        Serial.println(socketId);
        Serial.print("Pin number: ");
        Serial.println(pinNumber);
        Serial.print("State: ");
        Serial.println(state);
        Serial.print("Sensor Pin number: ");
        Serial.println(sensorPinNumber);

        relayPinNumbers.push_back(pinNumber);
        states.push_back(state);
        socketIds.push_back(socketId);
        sensorPinNumbers.push_back(sensorPinNumber);

        pos = nextPos != jsonArrayString.length() ? jsonArrayString.indexOf('{', nextPos + 1) : -1;
    }

}










void retrieveBoardCommands(int serialNumber) {
    Serial.println("Retrieving board commands...");
    WiFiClient client;
    if (!client.connect(EXPRESS_SERVER_IP, 3000)) {
        Serial.println("Connection to server failed!");
        return;
    }

    Serial.println("Connected to server");

    client.print("GET /WisewattsDeviceController/FindDeviceBySerialNumber/");
    client.print(serialNumber);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(EXPRESS_SERVER_IP);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    String response;

    while (client.connected()) {
        if (client.available()) {
            response = client.readString();
        }
    }

    Serial.print("Find device by serial number: ");
    Serial.println(response);

    int deviceId;
    int deviceIdIndex = response.indexOf("\"device_id\"");
    if (deviceIdIndex != -1) {
        int colonIndex = response.indexOf(":", deviceIdIndex);
        int commaIndex = response.indexOf(",", colonIndex);

        String deviceIdString = response.substring(colonIndex + 1, commaIndex);

        deviceId = deviceIdString.toInt();

        Serial.print("Device ID: ");
        Serial.println(deviceId);
    } else {
        Serial.println("Device ID not found in response");
    }

    client.stop();

//////////////////////////////////////////////////////////////

    if (!client.connect(EXPRESS_SERVER_IP, 3000)) {
        Serial.println("Connection to server failed!");
        return;
    }

    Serial.println("Connected to server");

    client.print("GET /CommandController/FindCommandByDeviceId/");
    client.print(deviceId);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(EXPRESS_SERVER_IP);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    String response2;

    while (client.connected()) {
        if (client.available()) {
            response2 = client.readString();
        }
    }

    Serial.println(response2);
    Serial.print("Commands by device id: ");

    client.stop();

    // Find the position of "[" indicating the start of the JSON array
    int startIndex = response2.indexOf("[");

    // Find the position of "]" indicating the end of the JSON array
    int endIndex = response2.indexOf("]");

    // Extract the substring containing the JSON array
    String jsonArrayString = response2.substring(startIndex + 1, endIndex);

    // Find the position of "{" indicating the start of the first object in the array
    int objStartIndex = jsonArrayString.indexOf("{");

    // Iterate over each object in the array
    while (objStartIndex != -1) {
        // Find the position of "socket_id" key
        int socketIdIndex = jsonArrayString.indexOf("\"socket_id\"", objStartIndex);
        if (socketIdIndex != -1) {
            // Find the position of ":" after "socket_id"
            int colonIndex = jsonArrayString.indexOf(":", socketIdIndex);

            // Find the position of "," after ":"
            int commaIndex = jsonArrayString.indexOf(",", colonIndex);

            // Extract the substring containing the socket_id value
            String socketIdString = jsonArrayString.substring(colonIndex + 1, commaIndex);

            // Convert the extracted substring to an integer
            int socketId = socketIdString.toInt();

            // Print the extracted socket_id
            Serial.print("Socket ID: ");
            Serial.println(socketId);

            for (size_t i = 0; i < socketIds.size(); i++) {
                if (socketIds[i] == socketId) {
                    states[i] = !states[i];
                    digitalWrite(relayPinNumbers[i], states[i]);
                }
            
                // Toggle the state of the sockets from the commands
                if (!client.connect(EXPRESS_SERVER_IP, 3000)) {
                    Serial.println("Connection to server failed!");
                    return;
                }

                Serial.println("Connected to server");

                client.print("PUT /SocketController/ToggleSocket/");
                client.print(socketId);
                client.print(" HTTP/1.1\r\n");
                client.print("Host: ");
                client.print(EXPRESS_SERVER_IP);
                client.print("\r\n");
                client.print("Connection: close\r\n\r\n");

                String response3;

                while (client.connected()) {
                    if (client.available()) {
                        response3 = client.readString();
                    }
                }

                Serial.print("Response: ");
                Serial.println(response3);
            }
        }

        // Find the position of "command_id" key
        int commandIdIndex = jsonArrayString.indexOf("\"command_id\"", objStartIndex);
        if (commandIdIndex != -1) {
            // Find the position of ":" after "command_id"
            int colonIndex = jsonArrayString.indexOf(":", commandIdIndex);

            // Find the position of "," after ":"
            int commaIndex = jsonArrayString.indexOf(",", colonIndex);

            // Extract the substring containing the command_id value
            String commandIdString = jsonArrayString.substring(colonIndex + 1, commaIndex);

            // Convert the extracted substring to an integer
            int commandId = commandIdString.toInt();

            // Print the extracted command_id
            Serial.print("Command ID: ");
            Serial.println(commandId);

            // Delete the command from the database
            if (!client.connect(EXPRESS_SERVER_IP, 3000)) {
                Serial.println("Connection to server failed!");
                return;
            }

            Serial.println("Connected to server");

            client.print("DELETE /CommandController/DeleteCommand/");
            client.print(commandId);
            client.print(" HTTP/1.1\r\n");
            client.print("Host: ");
            client.print(EXPRESS_SERVER_IP);
            client.print("\r\n");
            client.print("Connection: close\r\n\r\n");

            String response3;

            while (client.connected()) {
                if (client.available()) {
                    response3 = client.readString();
                }
            }

            Serial.print("Response: ");
            Serial.println(response3);

            client.stop();
        }

        // Find the position of the next object
        objStartIndex = jsonArrayString.indexOf("{", objStartIndex + 1);
    }

    // for (size_t i = 0; i < socketIds.size(); i++) {
    //     if(data[i]["device_id"] == deviceId) {
    //         if (socketIds[i] == data[i]["socket_id"]) {
    //             states[i] = !states[i];
    //             digitalWrite(relayPinNumbers[i], states[i]);
    //         }
    //     }
    // }
}







void sendCurrentData(int socketId, float current) {
    WiFiClient client;

    // Construct the JSON payload
    String payload = "{\"socket_id\":" + String(socketId) + ",\"current\":" + String(current) + "}";

    // Make the HTTP POST request
    if (!client.connect("your-express-server-address", 3000)) {
        Serial.println("Connection to server failed!");
        return;
    }

    client.print("POST /CurrentController/CreateCurrent HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(EXPRESS_SERVER_IP);
    client.print("\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(payload.length());
    client.print("\r\n\r\n");
    client.print(payload);

    // Read and print the response
    while (client.connected() || client.available()) {
        if (client.available()) {
            String response = client.readStringUntil('\r');
            Serial.println(response);
        }
    }

    client.stop();
}
