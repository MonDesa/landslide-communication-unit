#include "HealthMonitor.h"

HealthMonitor::HealthMonitor(AWSIoTManager &awsIoT, const String &commUnitID) : awsIoT(awsIoT), commUnitID(commUnitID) {}

void HealthMonitor::publishMetrics() {
    uint32_t freeHeap = ESP.getFreeHeap();
    String memoryUsage = String(freeHeap);

    String topic = "communication_unit/" + commUnitID + "/health/memory";
    if (awsIoT.publish(topic, memoryUsage)) {
        Serial.println("Health metrics published to AWS IoT Core");
    } else {
        Serial.println("Failed to publish health metrics");
    }
}
