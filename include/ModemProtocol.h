#pragma once

#include "ProtocolBase.h"

// The modem type (TINY_GSM_MODEM_SIM800) must be defined at compile-time due to library limitations.
// To support different modems, you might need to compile different firmware versions with the appropriate modem type defined.
#define TINY_GSM_MODEM_SIM800 // TODO(SAMUEL): Replace with modem model

#include <TinyGsmClient.h>

#define MODEM_RX 16 // TODO(SAMUEL): Adjust according to wiring
#define MODEM_TX 17 // TODO(SAMUEL): Adjust according to wiring

// TODO(SAMUEL): Define the serial for communication with the modem
HardwareSerial SerialAT(1); // Use UART1

class ModemProtocol : public ProtocolBase {
  public:
    ModemProtocol(const String &apn, const String &gprsUser, const String &gprsPass, const String &server, int port) : modem(SerialAT), apn_(apn), gprsUser_(gprsUser), gprsPass_(gprsPass), server_(server), port_(port) {}

    bool begin() override {
        SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

        modem.restart();

        if (!modem.waitForNetwork()) {
            Serial.println("Failed to connect to network");
            return false;
        }

        if (!modem.gprsConnect(apn_.c_str(), gprsUser_.c_str(), gprsPass_.c_str())) {
            Serial.println("Failed to connect to GPRS");
            return false;
        }

        return true;
    }

    bool isAvailable() override { return modem.isNetworkConnected(); }

    void sendData(const String &data) override {
        Serial.println("Sending data over modem: " + data);
        TinyGsmClient client(modem);

        if (!client.connect(server_.c_str(), port_)) {
            Serial.println("Connection to server failed");
            return;
        }

        client.print("POST /data HTTP/1.1\r\n");
        client.print("Host: ");
        client.print(server_);
        client.print("\r\n");
        client.print("Content-Type: application/json\r\n");
        client.print("Content-Length: ");
        client.print(data.length());
        client.print("\r\n\r\n");
        client.print(data);
        client.stop();
    }

    String getName() override { return "Modem"; }

  private:
    TinyGsm modem;
    String apn_;
    String gprsUser_;
    String gprsPass_;
    String server_;
    int port_;
};