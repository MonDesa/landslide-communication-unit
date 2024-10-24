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

  private:
    std::vector<ProtocolBase *> protocols;
};

#endif
