#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include "ExternalBrokerManager.h"
#include <Arduino.h>

class HealthMonitor {
  public:
    HealthMonitor(ExternalBrokerManager &externalBroker, const String &commUnitID);
    void publishMetrics();

  private:
    ExternalBrokerManager &externalBroker;
    String commUnitID;
};

#endif
