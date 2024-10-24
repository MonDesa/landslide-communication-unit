#ifndef LOCAL_BROKER_MANAGER_H
#define LOCAL_BROKER_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

class LocalBrokerManager {
  public:
    LocalBrokerManager(const String &brokerAddress, int brokerPort);
    bool connect();
    void collectData(JsonDocument &aggregatedData);
    void disconnect();

  private:
    String brokerAddress;
    int brokerPort;
    WiFiClient localClient;
    PubSubClient mqttClient;
    unsigned long collectionDuration = 10000; // 10 seconds
};

#endif
