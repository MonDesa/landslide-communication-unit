#pragma once

#include "ProtocolBase.h"
#include <HTTPClient.h>
#include <WiFi.h>

class WiFiProtocol : public ProtocolBase {
  public:
    WiFiProtocol(const String &ssid, const String &password) : ssid_(ssid), password_(password) {}

    bool begin() override {
        WiFi.begin(ssid_.c_str(), password_.c_str());
        int retries = 20;
        while (WiFi.status() != WL_CONNECTED && retries > 0) {
            delay(500);
            Serial.print(".");
            retries--;
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWi-Fi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            return true;
        } else {
            Serial.println("\nFailed to connect to Wi-Fi");
            return false;
        }
    }

    bool isAvailable() override { return (WiFi.status() == WL_CONNECTED); }

    void sendData(const String &data) override {
        Serial.println("Sending data over Wi-Fi: " + data);
        // TODO(SAMUEL): Implement data transmission over Wi-Fi (e.g., HTTP POST)
    }

    String getName() override { return "WiFi"; }

  private:
    String ssid_;
    String password_;
};
