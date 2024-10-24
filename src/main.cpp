#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#include "AWSIoTManager.h"
#include "ConfigManager.h"
#include "HealthMonitor.h"
#include "LoRaProtocol.h"
#include "LocalBrokerManager.h"
#include "ModemProtocol.h"
#include "ProtocolBase.h"
#include "ProtocolManager.h"
#include "WiFiProtocol.h"

const char *aws_endpoint = "aws-endpoint.amazonaws.com";
const char *aws_thingName = "ThingName";

const char *ca_cert = "-----BEGIN CERTIFICATE-----\n"
                      "YOUR_CA_CERTIFICATE_HERE\n"
                      "-----END CERTIFICATE-----\n";

const char *client_cert = "-----BEGIN CERTIFICATE-----\n"
                          "YOUR_CLIENT_CERTIFICATE_HERE\n"
                          "-----END CERTIFICATE-----\n";

const char *client_key = "-----BEGIN RSA PRIVATE KEY-----\n"
                         "YOUR_PRIVATE_KEY_HERE\n"
                         "-----END RSA PRIVATE KEY-----\n";

ConfigManager configManager;
AWSIoTManager awsIoT(aws_endpoint, aws_thingName, ca_cert, client_cert, client_key);
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

void postBrokerDataToAWS() {
    Serial.println("Posting all data from local MQTT broker to AWS IoT Core...");

    LocalBrokerManager localBroker(configManager.localBrokerAddress, configManager.localBrokerPort);
    if (localBroker.connect()) {
        JsonDocument aggregatedData;
        localBroker.collectData(aggregatedData);
        localBroker.disconnect();

        String aggregatedDataString;
        serializeJson(aggregatedData, aggregatedDataString);

        String topic = "communication_unit/" + configManager.CommUnitID + "/broker_data";
        if (awsIoT.publish(topic, aggregatedDataString)) {
            Serial.println("Aggregated broker data published to AWS IoT Core");
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

    // TODO(SAMUEL): Implement fetchConfiguration() if required, and fetch remote configuration if needed

    awsIoT.begin();

    while (!awsIoT.connect()) {
        // TODO(SAMUEL): Retry logic is handled in connect()
    }

    server.on("/config", HTTP_GET, handleGetConfig);
    server.on("/config", HTTP_POST, handlePostConfig);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");

    protocolManager.registerProtocol(new LoRaProtocol());
    protocolManager.registerProtocol(new ModemProtocol(configManager.apn, configManager.gprsUser, configManager.gprsPass, configManager.modemServer, configManager.modemPort));

    healthMonitor = new HealthMonitor(awsIoT, configManager.CommUnitID);

    healthMonitor->publishMetrics();
}

void loop() {
    awsIoT.loop();
    server.handleClient();

    // Should I fetch remote configuration periodically?

    if (millis() - lastMetricsTime > metricsInterval) {
        healthMonitor->publishMetrics();
        lastMetricsTime = millis();
    }

    if (millis() - lastDataPost > dataPostInterval) {
        postBrokerDataToAWS();
        lastDataPost = millis();
    }

    delay(10);
}
