#include "LocalBrokerManager.h"

LocalBrokerManager::LocalBrokerManager(const String &brokerAddress, int brokerPort) : brokerAddress(brokerAddress), brokerPort(brokerPort), mqttClient(localClient) { mqttClient.setServer(brokerAddress.c_str(), brokerPort); }

bool LocalBrokerManager::connect() {
    if (mqttClient.connect("DataCollector")) {
        Serial.println("Connected to local MQTT broker");
        mqttClient.subscribe("#");
        return true;
    } else {
        Serial.println("Failed to connect to local MQTT broker");
        return false;
    }
}

void LocalBrokerManager::collectData(JsonDocument &aggregatedData) {
    JsonArray dataArray = aggregatedData["data"].to<JsonArray>();

    mqttClient.setCallback([&](char *topic, byte *payload, unsigned int length) {
        String message;
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }

        JsonObject dataObject = dataArray.add<JsonObject>();
        dataObject["topic"] = String(topic);
        dataObject["message"] = message;
    });

    unsigned long startTime = millis();
    while (millis() - startTime < collectionDuration) {
        mqttClient.loop();
        delay(10);
    }
}

void LocalBrokerManager::disconnect() { mqttClient.disconnect(); }
