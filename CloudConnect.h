#include "secrets.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC "measure/basic"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

extern WiFiClientSecure net;
extern MQTTClient client;

void messageHandler(String &topic, String &payload);

void connectToCloud();

void publishMessage();
