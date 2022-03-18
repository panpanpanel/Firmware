#include "CloudConnect.h"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(512);

void connectToCloud()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected");

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IoT ");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(int door_latched, int voltage, int currents[8])
{
  StaticJsonDocument<512> doc;
  doc["device_id"] = analogRead(0);
  //doc["time"] = millis();
  doc["is_door_latched"] = door_latched;
  doc["voltage"] = voltage;
  for (int i = 0 ; i < 8 ; i++) {
    String a = "breaker_";
    String c = "_current";
    String header = a + i + c;
    doc[header] = currents[i];
  }

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  client.loop();
  Serial.println ("published");
  delay(10);
}
