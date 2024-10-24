#include "AWSIoTManager.h"

AWSIoTManager::AWSIoTManager(const char *endpoint, const char *thingName, const char *caCert, const char *clientCert, const char *clientKey) : aws_endpoint(endpoint), aws_thingName(thingName), ca_cert(caCert), client_cert(clientCert), client_key(clientKey), mqttClient(netClient) {}

void AWSIoTManager::begin() {
    netClient.setCACert(ca_cert);
    netClient.setCertificate(client_cert);
    netClient.setPrivateKey(client_key);

    mqttClient.setServer(aws_endpoint, 8883);
}

bool AWSIoTManager::connect() {
    if (!mqttClient.connected()) {
        Serial.print("Connecting to AWS IoT Core...");
        if (mqttClient.connect(aws_thingName)) {
            Serial.println("Connected to AWS IoT Core");
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

void AWSIoTManager::loop() { mqttClient.loop(); }

bool AWSIoTManager::publish(const String &topic, const String &payload) { return mqttClient.publish(topic.c_str(), payload.c_str()); }

void AWSIoTManager::setCallback(MQTT_CALLBACK_SIGNATURE) { mqttClient.setCallback(callback); }
