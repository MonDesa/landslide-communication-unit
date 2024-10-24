#pragma once

#include "ProtocolBase.h"
#include <LoRa.h>

#define CS_PIN 18  // TODO(SAMUEL): Adjust according to wiring
#define RST_PIN 14 // TODO(SAMUEL): Adjust according to wiring
#define IRQ_PIN 26 // TODO(SAMUEL): Adjust according to wiring

class LoRaProtocol : public ProtocolBase {
  public:
    bool begin() override {
        LoRa.setPins(CS_PIN, RST_PIN, IRQ_PIN);
        bool success = LoRa.begin(915E6); // Lora frequency
        return success;
    }

    bool isAvailable() override {
        return true; // TODO(SAMUEL): Implement availability check
    }

    void sendData(const String &data) override {
        Serial.println("Sending data over LoRa: " + data);
        LoRa.beginPacket();
        LoRa.print(data);
        LoRa.endPacket();
    }

    String getName() override { return "LoRa"; }
};
