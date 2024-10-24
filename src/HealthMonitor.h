#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include "AWSIoTManager.h"
#include <Arduino.h>

class HealthMonitor {
  public:
    HealthMonitor(AWSIoTManager &awsIoT, const String &commUnitID);
    void publishMetrics();

  private:
    AWSIoTManager &awsIoT;
    String commUnitID;
};

#endif
