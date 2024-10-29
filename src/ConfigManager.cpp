#include "ConfigManager.h"

bool ConfigManager::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return false;
    }
    readConfig();
    return true;
}

void ConfigManager::readConfig() {
    File file = SPIFFS.open(configFilePath, "r");
    if (!file) {
        Serial.println("Failed to open config file");
        return;
    }
    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size + 1]);
    file.readBytes(buf.get(), size);
    buf[size] = '\0';
    file.close();

    DynamicJsonDocument doc(4096);
    auto error = deserializeJson(doc, buf.get());
    if (error) {
        Serial.println("Failed to parse config file");
        return;
    }

    CommUnitID = doc["CommUnitID"].as<String>();
    ssid = doc["wifi"]["ssid"].as<String>();
    password = doc["wifi"]["password"].as<String>();
    apn = doc["modem"]["apn"].as<String>();
    gprsUser = doc["modem"]["gprsUser"].as<String>();
    gprsPass = doc["modem"]["gprsPass"].as<String>();
    modemServer = doc["modem"]["server"].as<String>();
    modemPort = doc["modem"]["port"];
    adminUsername = doc["admin"]["username"].as<String>();
    adminPassword = doc["admin"]["password"].as<String>();
    localBrokerAddress = doc["mqtt"]["localBrokerAddress"].as<String>();
    localBrokerPort = doc["mqtt"]["localBrokerPort"] | 1883;
    externalBrokerAddress = doc["externalBroker"]["address"].as<String>();
    externalBrokerPort = doc["externalBroker"]["port"] | 1883;
}

void ConfigManager::saveConfig(const String &jsonString) {
    File file = SPIFFS.open(configFilePath, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return;
    }
    file.print(jsonString);
    file.close();
}
