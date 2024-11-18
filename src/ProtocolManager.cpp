#include "ProtocolManager.h"

ProtocolManager::~ProtocolManager() {
    clearProtocols();
}

void ProtocolManager::registerProtocol(ProtocolBase *protocol) {
    if (protocol->begin()) {
        if (protocol->isAvailable()) {
            protocols.push_back(protocol);
            Serial.println(protocol->getName() + " protocol registered.");
        } else {
            Serial.println(protocol->getName() + " protocol not available.");
            delete protocol;
        }
    } else {
        Serial.println("Failed to initialize " + protocol->getName() + " protocol.");
        delete protocol;
    }
}

const std::vector<ProtocolBase *> &ProtocolManager::getProtocols() const { return protocols; }

void ProtocolManager::loop() {
    for (ProtocolBase *protocol : protocols) {
        protocol->loop(); // Call loop on each protocol
    }
}

void ProtocolManager::clearProtocols() {
    for (ProtocolBase *protocol : protocols) {
        delete protocol;
    }
    protocols.clear();
}
