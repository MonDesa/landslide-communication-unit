#ifndef PROTOCOL_BASE_H
#define PROTOCOL_BASE_H

#include <Arduino.h>

class ProtocolBase {
  public:
    virtual ~ProtocolBase() {}
    virtual bool begin() = 0;
    virtual bool isAvailable() = 0;
    virtual void sendData(const String &data) = 0;
    virtual void loop() = 0; // Added loop method
    virtual String getName() = 0;
};

#endif
