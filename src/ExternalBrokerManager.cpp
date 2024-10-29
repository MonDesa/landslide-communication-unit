#include "ExternalBrokerManager.h"

ExternalBrokerManager::ExternalBrokerManager(const String &brokerAddress, int brokerPort) : brokerAddress(brokerAddress), brokerPort(brokerPort), mqttClient(netClient) {}

void ExternalBrokerManager::begin() { mqttClient.setServer(brokerAddress.c_str(), brokerPort); }

bool ExternalBrokerManager::connect() {
    if (!mqttClient.connected()) {
        Serial.print("Connecting to External MQTT Broker...");
        if (mqttClient.connect("CommUnit")) {
            Serial.println("Connected to External MQTT Broker");
            return true;
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Trying again in 5 seconds");
            delay(5000);
            return false;
        }
    }
    return true;
}

void ExternalBrokerManager::loop() { mqttClient.loop(); }

bool ExternalBrokerManager::publish(const String &topic, const String &payload) { return mqttClient.publish(topic.c_str(), payload.c_str()); }

void ExternalBrokerManager::setCallback(MQTT_CALLBACK_SIGNATURE) { mqttClient.setCallback(callback); }
