#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

// #include "hal/temperature_sensor_types.h"


const char *ssid = "<>";
const char *passwd = "<>";
const char *mqtt_broker = "broker.mosquitto.io";
const char *topic = "test";

const int mqtt_port = 1883;
const char* mqtt_server = "192.168.0.244";

WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi() {
  WiFi.begin(ssid, passwd);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to esp:......");
    delay(500);
  }
  Serial.println("Connected:....!!");
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.publish("esp32/status", "Connected");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  
  float sensor_temp = temperatureRead();
  char sensor_temp_str[20];
  snprintf(sensor_temp_str, sizeof(sensor_temp_str), "%.2f", sensor_temp);

  client.loop();

  client.publish("esp32/data", "Sensor data: ");
  delay(200);
  client.publish("esp32/data", sensor_temp_str);
  delay(5000);
}