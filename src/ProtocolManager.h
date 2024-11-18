#ifndef PROTOCOL_MANAGER_H
#define PROTOCOL_MANAGER_H

#include "ProtocolBase.h"
#include <Arduino.h>
#include <vector>

class ProtocolManager {
  public:
    ~ProtocolManager();
    void registerProtocol(ProtocolBase *protocol);
    const std::vector<ProtocolBase *> &getProtocols() const;
    void loop();            // Added loop method
    void clearProtocols();  // Added clearProtocols method

  private:
    std::vector<ProtocolBase *> protocols;
};

#endif
