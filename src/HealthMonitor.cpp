#include "HealthMonitor.h"

HealthMonitor::HealthMonitor(ExternalBrokerManager &externalBroker, const String &commUnitID) : externalBroker(externalBroker), commUnitID(commUnitID) {}

void HealthMonitor::publishMetrics() {
    uint32_t freeHeap = ESP.getFreeHeap();
    String memoryUsage = String(freeHeap);

    String topic = "communication_unit/" + commUnitID + "/health/memory";
    if (externalBroker.publish(topic, memoryUsage)) {
        Serial.println("Health metrics published to External Broker");
    } else {
        Serial.println("Failed to publish health metrics");
    }
}
