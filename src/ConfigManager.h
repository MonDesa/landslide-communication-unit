#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

class ConfigManager {
  public:
    bool begin();
    void readConfig();
    void saveConfig(const String &jsonString);

    String CommUnitID;
    String ssid;
    String password;
    String apn;
    String gprsUser;
    String gprsPass;
    String modemServer;
    int modemPort;
    String adminUsername;
    String adminPassword;
    String localBrokerAddress;
    int localBrokerPort;

  private:
    const char *configFilePath = "/config.json";
};

#endif
