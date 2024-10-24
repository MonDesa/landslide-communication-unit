#ifndef AWS_IOT_MANAGER_H
#define AWS_IOT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class AWSIoTManager {
  public:
    AWSIoTManager(const char *endpoint, const char *thingName, const char *caCert, const char *clientCert, const char *clientKey);
    void begin();
    bool connect();
    void loop();
    bool publish(const String &topic, const String &payload);
    void setCallback(MQTT_CALLBACK_SIGNATURE);

  private:
    const char *aws_endpoint;
    const char *aws_thingName;
    const char *ca_cert;
    const char *client_cert;
    const char *client_key;

    WiFiClientSecure netClient;
    PubSubClient mqttClient;
};

#endif
