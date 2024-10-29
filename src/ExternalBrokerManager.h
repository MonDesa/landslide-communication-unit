#ifndef EXTERNAL_BROKER_MANAGER_H
#define EXTERNAL_BROKER_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

class ExternalBrokerManager {
  public:
    ExternalBrokerManager(const String &brokerAddress, int brokerPort);
    void begin();
    bool connect();
    void loop();
    bool publish(const String &topic, const String &payload);
    void setCallback(MQTT_CALLBACK_SIGNATURE);

  private:
    String brokerAddress;
    int brokerPort;
    WiFiClient netClient;
    PubSubClient mqttClient;
};

#endif
