#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h> // MQTT library

#include "ConfigManager.h"
#include "ExternalBrokerManager.h"
#include "HealthMonitor.h"
#include "LoRaProtocol.h"
#include "LocalBrokerManager.h"
#include "ModemProtocol.h"
#include "ProtocolBase.h"
#include "ProtocolManager.h"
#include "WiFiProtocol.h"

// Wi-Fi and MQTT clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

ConfigManager configManager;
ExternalBrokerManager externalBroker("", 1883); // placeholder, will set in setup()
ProtocolManager protocolManager;
HealthMonitor *healthMonitor = nullptr;

// Timing variables
unsigned long lastDataPost = 0;
const unsigned long dataPostInterval = 86400000; // 24 hours
unsigned long lastMetricsTime = 0;
const unsigned long metricsInterval = 10000; // 10 seconds

void reinitializeServices() {
    // Reinitialize the external broker if needed
    externalBroker = ExternalBrokerManager(configManager.externalBrokerAddress, configManager.externalBrokerPort);
    externalBroker.begin();

    while (!externalBroker.connect()) {
        // Retry logic is handled in connect()
    }

    // Reinitialize Wi-Fi if credentials have changed
    if (WiFi.SSID() != configManager.ssid) {
        Serial.println("Wi-Fi credentials have changed. Reconnecting...");

        WiFi.disconnect();
        delay(1000);

        WiFi.begin(configManager.ssid.c_str(), configManager.password.c_str());

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nConnected to Wi-Fi");
    }

    // Reinitialize other protocols if necessary
    protocolManager.clearProtocols();
    protocolManager.registerProtocol(new WiFiProtocol(configManager.ssid, configManager.password));
    // Register other protocols based on the updated configuration
    if (configManager.enableLoRa) {
        protocolManager.registerProtocol(new LoRaProtocol());
    }
    if (configManager.enableModem) {
        protocolManager.registerProtocol(new ModemProtocol(
            configManager.apn,
            configManager.gprsUser,
            configManager.gprsPass,
            configManager.modemServer,
            configManager.modemPort
        ));
    }

    // Re-subscribe to MQTT topics
    String configTopic = "comm_unit/" + configManager.CommUnitID + "/config";
    mqttClient.subscribe(configTopic.c_str());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("Received message on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);

    // Check if the message is on the configuration topic
    String configTopic = "comm_unit/" + configManager.CommUnitID + "/config";
    if (String(topic) == configTopic) {
        // Update configuration
        if (configManager.saveConfig(message)) {
            Serial.println("Configuration updated successfully.");

            configManager.readConfig();

            reinitializeServices();

            // Publish status
            String statusTopic = "comm_unit/" + configManager.CommUnitID + "/config/status";
            String statusMessage = "{\"status\":\"success\"}";
            mqttClient.publish(statusTopic.c_str(), statusMessage.c_str());
        } else {
            Serial.println("Failed to save configuration.");

            // Publish status
            String statusTopic = "comm_unit/" + configManager.CommUnitID + "/config/status";
            String statusMessage = "{\"status\":\"failure\"}";
            mqttClient.publish(statusTopic.c_str(), statusMessage.c_str());
        }
    }
}

void setup() {
    Serial.begin(115200);

    if (!configManager.begin()) {
        Serial.println("Configuration Manager failed to start");
        return;
    }

    // Connect to Wi-Fi
    WiFi.begin(configManager.ssid.c_str(), configManager.password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize MQTT client
    mqttClient.setServer(configManager.externalBrokerAddress.c_str(), configManager.externalBrokerPort);
    mqttClient.setCallback(mqttCallback);

    // Connect to MQTT broker
    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT broker...");
        if (mqttClient.connect(configManager.CommUnitID.c_str())) {
            Serial.println("connected");

            // Subscribe to the config topic
            String configTopic = "comm_unit/" + configManager.CommUnitID + "/config";
            mqttClient.subscribe(configTopic.c_str());
            Serial.print("Subscribed to topic: ");
            Serial.println(configTopic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" trying again in 5 seconds");
            delay(5000);
        }
    }

    protocolManager.registerProtocol(new WiFiProtocol(configManager.ssid, configManager.password));

    if (configManager.enableLoRa) {
        protocolManager.registerProtocol(new LoRaProtocol());
    }

    if (configManager.enableModem) {
        protocolManager.registerProtocol(new ModemProtocol(
            configManager.apn,
            configManager.gprsUser,
            configManager.gprsPass,
            configManager.modemServer,
            configManager.modemPort
        ));
    }

    // Initialize External Broker Manager
    externalBroker = ExternalBrokerManager(configManager.externalBrokerAddress, configManager.externalBrokerPort);
    externalBroker.begin();

    while (!externalBroker.connect()) {
        // Retry logic is handled in connect()
    }

    // Initialize Health Monitor
    healthMonitor = new HealthMonitor(externalBroker, configManager.CommUnitID);
    healthMonitor->publishMetrics();

    Serial.println("Setup completed");
}

void loop() {
    if (!mqttClient.connected()) {
        while (!mqttClient.connected()) {
            Serial.print("Reconnecting to MQTT broker...");
            if (mqttClient.connect(configManager.CommUnitID.c_str())) {
                Serial.println("connected");
                String configTopic = "comm_unit/" + configManager.CommUnitID + "/config";
                mqttClient.subscribe(configTopic.c_str());
                Serial.print("Subscribed to topic: ");
                Serial.println(configTopic);
            } else {
                Serial.print("failed, rc=");
                Serial.print(mqttClient.state());
                Serial.println(" trying again in 5 seconds");
                delay(5000);
            }
        }
    }

    mqttClient.loop();

    // Handle external broker
    externalBroker.loop();

    // Handle protocols
    protocolManager.loop();

    // Publish health metrics periodically
    if (millis() - lastMetricsTime > metricsInterval) {
        healthMonitor->publishMetrics();
        lastMetricsTime = millis();
    }

    // Post data from local broker to external broker periodically
    if (millis() - lastDataPost > dataPostInterval) {
        postBrokerDataToExternalBroker();
        lastDataPost = millis();
    }

    delay(10);
}

void postBrokerDataToExternalBroker() {
    Serial.println("Posting all data from local MQTT broker to External Broker...");

    LocalBrokerManager localBroker(configManager.localBrokerAddress, configManager.localBrokerPort);
    if (localBroker.connect()) {
        // Collect data from the local broker
        DynamicJsonDocument aggregatedData(2048);
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
