#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#include "ConfigManager.h"
#include "ExternalBrokerManager.h"
#include "HealthMonitor.h"
#include "LoRaProtocol.h"
#include "LocalBrokerManager.h"
#include "ModemProtocol.h"
#include "ProtocolBase.h"
#include "ProtocolManager.h"
#include "WiFiProtocol.h"

ConfigManager configManager;
ExternalBrokerManager externalBroker("", 1883); // Placeholder, will set in setup()
ProtocolManager protocolManager;
HealthMonitor *healthMonitor = nullptr;
WebServer server(80);

unsigned long lastConfigFetch = 0;
const unsigned long configFetchInterval = 3600000; // 1 hour
unsigned long lastDataPost = 0;
const unsigned long dataPostInterval = 86400000; // 24 hours
unsigned long lastMetricsTime = 0;
const unsigned long metricsInterval = 10000; // 10 seconds

void handleGetConfig() {
    if (!server.authenticate(configManager.adminUsername.c_str(), configManager.adminPassword.c_str())) {
        return server.requestAuthentication();
    }
    // TODO(SAMUEL): Implement the logic to send the config
}

void handlePostConfig() {
    if (!server.authenticate(configManager.adminUsername.c_str(), configManager.adminPassword.c_str())) {
        return server.requestAuthentication();
    }
    // TODO(SAMUEL): Implement the logic to receive and save the config
}

void handleNotFound() { server.send(404, "application/json", "{\"status\":\"Not Found\"}"); }

void postBrokerDataToExternalBroker() {
    Serial.println("Posting all data from local MQTT broker to External Broker...");

    LocalBrokerManager localBroker(configManager.localBrokerAddress, configManager.localBrokerPort);
    if (localBroker.connect()) {
        DynamicJsonDocument aggregatedData(8192); // Adjust size as needed
        localBroker.collectData(aggregatedData);
        localBroker.disconnect();

        String aggregatedDataString;
        serializeJson(aggregatedData, aggregatedDataString);

        String topic = "communication_unit/" + configManager.CommUnitID + "/broker_data";
        if (externalBroker.publish(topic, aggregatedDataString)) {
            Serial.println("Aggregated broker data published to External Broker");
        } else {
            Serial.println("Failed to publish aggregated broker data");
        }
    }
}

void setup() {
    Serial.begin(115200);

    if (!configManager.begin()) {
        Serial.println("Configuration Manager failed to start");
        return;
    }

    Serial.print("Communication Unit ID: ");
    Serial.println(configManager.CommUnitID);

    protocolManager.registerProtocol(new WiFiProtocol(configManager.ssid, configManager.password));

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to Wi-Fi via WiFiProtocol");
        // TODO(SAMUEL): Handle failure
        return;
    }

    externalBroker = ExternalBrokerManager(configManager.externalBrokerAddress, configManager.externalBrokerPort);
    externalBroker.begin();

    while (!externalBroker.connect()) {
        // TODO(SAMUEL): Retry logic is handled in connect()
    }

    server.on("/config", HTTP_GET, handleGetConfig);
    server.on("/config", HTTP_POST, handlePostConfig);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");

    protocolManager.registerProtocol(new LoRaProtocol());
    protocolManager.registerProtocol(new ModemProtocol(configManager.apn, configManager.gprsUser, configManager.gprsPass, configManager.modemServer, configManager.modemPort));

    healthMonitor = new HealthMonitor(externalBroker, configManager.CommUnitID);

    healthMonitor->publishMetrics();
}

void loop() {
    externalBroker.loop();
    server.handleClient();

    if (millis() - lastMetricsTime > metricsInterval) {
        healthMonitor->publishMetrics();
        lastMetricsTime = millis();
    }

    if (millis() - lastDataPost > dataPostInterval) {
        postBrokerDataToExternalBroker();
        lastDataPost = millis();
    }

    delay(10);
}
